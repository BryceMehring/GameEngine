#ifndef _ONCEFSM_
#define _ONCEFSM_

// todo: this needs to be moved, and renamed?
class OnceFSM
{
public:

	// Constructor
	OnceFSM() : m_state(false) {}

	// Access
	bool GetState() const { return m_state; }

	// compare's operator () must return a bool
	// Run does work() only once until (compare() == false)
	template< class F, class F2>
	void Run(const F& compare, const F2& work)
	{
		if(compare())
		{
			if(m_state == false)
			{
				work();

				m_state = true;
			}
		}
		else
		{
			m_state = false;
		}
	}

private:

	bool m_state;

};

#endif // _ONCEFSM_