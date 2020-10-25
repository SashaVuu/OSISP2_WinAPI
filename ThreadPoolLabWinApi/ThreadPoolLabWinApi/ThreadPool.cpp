#include "ThreadPool.h"


ThreadPool::ThreadPool(int amountOfThreads)
{

	logger = new Logger("./logger.txt");

	currentThreadAmount = amountOfThreads;			
	currentWorkingThreadAmount = 0;

	
	InitializeCriticalSection(&threadQueueCrSection);
	InitializeConditionVariable(&threadQueueCnVariable);

	InitializeCriticalSection(&taskQueueCrSection);
	InitializeConditionVariable(&taskQueueCnVariable);

	//Создание потока управления
	isManagerAlive = true;
	managerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startManager, this, 0, NULL);


	//Создание потоков исполнения
	threads = (HANDLE*)calloc(MAX_THREAD_AMOUNT, sizeof(HANDLE));
	for (int i = 0; i < currentThreadAmount; i++) {
		threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startThread, this, 0, NULL);
	}
	

	logger->Write("Initialized system.",GetCurrentThreadId());
}


ThreadPool::~ThreadPool()
{
	logger->Write("1.Destructor started", GetCurrentThreadId());

	isPoolAlive = false;
	WakeAllConditionVariable(&taskQueueCnVariable);

	//Ждем окончание работы потока управления
	WaitForSingleObject(managerThread, INFINITE);

	logger->Write("2.Manager Died", GetCurrentThreadId());

	//Будим все конд вар, что бы выйти со слипа
	WakeAllConditionVariable(&threadQueueCnVariable);

	for (int i = 0; i < threadsQueue.size(); i++) {
		Thread* thread = threadsQueue.front();
		WakeAllConditionVariable(&thread->conditionalVariable);
		threadsQueue.pop();
		threadsQueue.push(thread);
	}

	//Ждем окончание работы всех потоков
	WaitForMultipleObjects(currentThreadAmount, threads, TRUE, INFINITE);

	logger->Write("3.All threads Died", GetCurrentThreadId());

	//Удаляем объекты
	while (!threadsQueue.empty()) {
		DeleteCriticalSection(&threadsQueue.front()->criticalSection);
		threadsQueue.pop();
	}

	for (int i = 0; i < currentThreadAmount; i++) {
		CloseHandle(threads[i]);
	}

	DeleteCriticalSection(&threadQueueCrSection);
	DeleteCriticalSection(&taskQueueCrSection);
	CloseHandle(managerThread);
	logger->Write("ThreadPool been destroyed.", GetCurrentThreadId());
}



void ThreadPool::AddTask(function<void()> func)
{

	//Добавляем задачу в очередь на исполнение
	EnterCriticalSection(&taskQueueCrSection);
	taskQueue.push(func);
	WakeConditionVariable(&taskQueueCnVariable);
	LeaveCriticalSection(&taskQueueCrSection);


	logger->Write("Added task to a queue.", GetCurrentThreadId());

}



DWORD WINAPI ThreadPool::startThread(ThreadPool* pool)
{
	Thread* thread = new Thread();

	//Добавляем в очередь потоков, созданный поток
	EnterCriticalSection(&pool->threadQueueCrSection);
	pool->threadsQueue.push(thread);
	WakeConditionVariable(&pool->threadQueueCnVariable);
	LeaveCriticalSection(&pool->threadQueueCrSection);

	pool->logger->Write("Thread been initialized.", GetCurrentThreadId());

	//Пока жив менеджер или у потока еще есть задача на выполнение
	while (pool->isManagerAlive|| thread->task!=NULL) {
		
		EnterCriticalSection(&thread->criticalSection);

		//Ожидание задачи на выполнение
		while (thread->task == NULL && pool->isManagerAlive)
			SleepConditionVariableCS(&thread->conditionalVariable, &thread->criticalSection, INFINITE);

		//if (thread->task != NULL) {

			//Увеличиваем значение работающих потоков
			InterlockedExchange(&pool->currentWorkingThreadAmount, pool->currentWorkingThreadAmount + 1);
			try {
				thread->task();
			}
			catch (exception ex) {
				pool->logger->Write(ex.what(), GetCurrentThreadId());
			}

			pool->logger->Write("Task was executed by this thread.", GetCurrentThreadId());

			//Уменьшаем количество работающих потоков
			InterlockedExchange(&pool->currentWorkingThreadAmount, pool->currentWorkingThreadAmount - 1);
			thread->task = NULL;

		//}
		LeaveCriticalSection(&thread->criticalSection);

	}

	pool->logger->Write("Thread procedure is over.", GetCurrentThreadId());

	return 0;
}




DWORD WINAPI ThreadPool::startManager(ThreadPool* pool)
{

	//Ожидание создания всех потоков 
	EnterCriticalSection(&pool->threadQueueCrSection);
	while (pool->threadsQueue.size() < pool->currentThreadAmount)
		SleepConditionVariableCS(&pool->threadQueueCnVariable, &pool->threadQueueCrSection, INFINITE);
	LeaveCriticalSection(&pool->threadQueueCrSection);

	pool->logger->Write("Manager was initialized.", GetCurrentThreadId());
	
	
	while (pool->isPoolAlive || !pool->taskQueue.empty()) {

	
		EnterCriticalSection(&pool->taskQueueCrSection);

		while (pool->taskQueue.empty() && pool->isPoolAlive)
			SleepConditionVariableCS(&pool->taskQueueCnVariable, &pool->taskQueueCrSection, 2000);
		

		if (!pool->taskQueue.empty()) {

			//Вытаскиваем из очереди задачу
			function<void()> task = pool->taskQueue.front();
			pool->taskQueue.pop();

			LeaveCriticalSection(&pool->taskQueueCrSection);


			//Находим свободный поток
			EnterCriticalSection(&pool->threadQueueCrSection);

			pool->logger->Write(" - amount of working threads", pool->currentWorkingThreadAmount);

			//Подбираем свободный поток
			if (pool->currentWorkingThreadAmount< pool->currentThreadAmount)
					
					while (pool->threadsQueue.front()->task != NULL) {
						Thread* thread = pool->threadsQueue.front();
						pool->threadsQueue.pop();
						pool->threadsQueue.push(thread);
			}

			//Если все потоки заняты, то ждем освободившегося
			else if (pool->currentWorkingThreadAmount == pool->currentThreadAmount)
			{
					pool->logger->Write("All threads are busy,waiting for free one...", GetCurrentThreadId());
					while (!pool->WaitForOneThread()) { Sleep(1000); };

			}

			pool->threadsQueue.front()->task = task;
			pool->logger->Write("Manager choosed thread to execute new task", GetCurrentThreadId());

			//Будим поток, так как для него появилась работа
			WakeConditionVariable(&pool->threadsQueue.front()->conditionalVariable);

			LeaveCriticalSection(&pool->threadQueueCrSection);
		}
		
	}

	//Ждем пока все потоки закончат выполнение своих задач
	while (!pool->WaitForAllThreads()) 
	{
		Sleep(1000);
	}

	InterlockedExchange(&pool->isManagerAlive, FALSE);
	pool->logger->Write("Manager procedure is over.", GetCurrentThreadId());

	return 0;
}


bool ThreadPool::WaitForAllThreads()
{
	EnterCriticalSection(&threadQueueCrSection);
	bool isEveryoneFree = true;
	for (int i = 0; i < threadsQueue.size(); i++) {
		Thread* thread = threadsQueue.front();
		threadsQueue.pop();
		threadsQueue.push(thread);

		isEveryoneFree = isEveryoneFree && (thread->task == NULL);
	}

	LeaveCriticalSection(&threadQueueCrSection);
	return isEveryoneFree;

}

bool ThreadPool::WaitForOneThread()
{

	EnterCriticalSection(&threadQueueCrSection);
	bool isAnyоneFree = false;
	while (!isAnyоneFree) {
		for (int i = 0; i < threadsQueue.size(); i++) {
			Thread* thread = threadsQueue.front();
			threadsQueue.pop();
			threadsQueue.push(thread);
			if (thread->task == NULL) { isAnyоneFree = true; }
		}
	}
	LeaveCriticalSection(&threadQueueCrSection);
	return isAnyоneFree;

}