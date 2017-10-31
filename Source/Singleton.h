#ifndef SINGLETON_H_
#define SINGLETON_H_

template<class T>
class Singleton
{
public:
	static T* Instance()
	{
		if (m_pInstance == NULL)
		{
			m_pInstance = new T();
		}
		return m_pInstance;
	}

protected:
	Singleton() {}
	Singleton(const Singleton&) {}
	Singleton& operator=(const Singleton&) {}

	virtual ~Singleton() {}

	static void Destroy()
	{
		if (m_pInstance)
		{
			delegate m_pInstance;
			m_pInstance = NULL£»
		}
	}
private:
	static T* m_pInstance;
};
template<class T> T* Singleton<T>::m_pInstance = NULL;

#endif //SINGLETON_H_