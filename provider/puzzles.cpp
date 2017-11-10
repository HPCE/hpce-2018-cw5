#ifndef local_puzzles_hpp
#define local_puzzles_hpp

#include "hold_time.hpp"
#include "gaussian_blur.hpp"
#include "edit_distance.hpp"
#include "random_projection.hpp"
#include "mining.hpp"
#include "heat_world.hpp"

void puzzler::PuzzleRegistrar::UserRegisterPuzzles()
{
  Register(std::make_shared<HoldTimeProvider>());
  Register(std::make_shared<GaussianBlurProvider>());
  Register(std::make_shared<EditDistanceProvider>());
  Register(std::make_shared<RandomProjectionProvider>());
  Register(std::make_shared<MiningProvider>());
  Register(std::make_shared<HeatWorldProvider>());
}


#endif
