#ifndef user_hold_time_hpp
#define user_hold_time_hpp

#include "puzzler/puzzles/hold_time.hpp"


class HoldTimeProvider
  : public puzzler::HoldTimePuzzle
{
public:
  HoldTimeProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::HoldTimeInput *input,
				puzzler::HoldTimeOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
