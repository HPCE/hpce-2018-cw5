#ifndef user_ising_hpp
#define user_ising_hpp

#include "puzzler/puzzles/ising.hpp"


class IsingProvider
  : public puzzler::IsingPuzzle
{
public:
  IsingProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::IsingInput *input,
				 puzzler::IsingOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
