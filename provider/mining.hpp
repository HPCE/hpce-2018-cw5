#ifndef user_mining_hpp
#define user_mining_hpp

#include "puzzler/puzzles/mining.hpp"


class MiningProvider
  : public puzzler::MiningPuzzle
{
public:
  MiningProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::MiningInput *input,
				puzzler::MiningOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
