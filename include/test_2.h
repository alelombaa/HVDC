/*!
\file test_2.h
Test case with uniform sigma and two different values for epsilon, each for half of the domain wrt the z axes
*/
#include <tmesh_3d.h>
#include <simple_connectivity_3d_thin.h>

constexpr int NUM_REFINEMENTS = 4;
constexpr int maxlevel = 4;

constexpr double DELTAT = 50.0;
constexpr double T = 5000;
constexpr double tau = 50.0; 
constexpr bool save_sol = true;

// Problem parameters
constexpr double epsilon_0 = 8.8542e-12;
constexpr double epsilon_r_1 = 2.0;         // permittivity bottom half
constexpr double epsilon_r_2 = 4.0;         // permittivity upper half
constexpr double sigma_ = 3.21e-14;           // conducivity coeff

constexpr size_t N_rhos = 3;
std::vector<size_t> rho_idx;
std::vector<std::vector<double>> points{{0.0005,0.0005,0.0},{0.0005,0.0005,0.0005},{0.0005,0.0005,0.001}};
std::vector<std::vector<double>> tols{{1e-4,1e-4,1e-9},{1e-4,1e-4,1e-9},{1e-4,1e-4,1e-9}};

bool extra_refinement = true;

static int
uniform_refinement (tmesh_3d::quadrant_iterator q)
{ return NUM_REFINEMENTS; }

static int
refinement (tmesh_3d::quadrant_iterator quadrant)
{
  int currentlevel = static_cast<int> (quadrant->the_quadrant->level);
  double zcoord;
  int retval = 0;
  for (int ii = 0; ii < 8; ++ii)
    {
      zcoord = quadrant->p(2, ii);
      
      if (fabs(zcoord - 0.0005) < 1e-9)
        {
          retval = maxlevel - currentlevel;
          break;
        }
    }

  if (currentlevel >= maxlevel)
    retval = 0;
      
  return retval;
}

static int
coarsening (tmesh_3d::quadrant_iterator quadrant)
{
  int currentlevel = static_cast<int> (quadrant->the_quadrant->level);
  double zcoord;
  int retval = currentlevel - NUM_REFINEMENTS;
  for (int ii = 0; ii < 8; ++ii)
    {     
      zcoord = quadrant->p(2, ii);

      if (fabs(zcoord - 0.0005) < 1e-9)
        {
          retval = 0;
          break;
        }
    }

  if (currentlevel <= NUM_REFINEMENTS)
    retval = 0;
      
  return (retval);
}

double epsilon_fun(const double & x, const double & y, const double & z)
{return z < 0.0005 ? epsilon_0 * epsilon_r_1 : epsilon_0 * epsilon_r_2;}

double sigma_fun(const double & x, const double & y, const double & z)
{return sigma_ * DELTAT;}

std::vector<size_t> find_idx(tmesh_3d &tmsh,std::vector<std::vector<double>> &points,std::vector<std::vector<double>> &tols, const size_t &N_rhos)
{
  std::vector<size_t> id(N_rhos,0);

  for (size_t i = 0; i < N_rhos; i++) {
    bool found = false;

    for (auto quadrant = tmsh.begin_quadrant_sweep ();
       quadrant != tmsh.end_quadrant_sweep ();
       ++quadrant){

        for (int ii = 0; ii < 8; ++ii) {

          if (fabs(quadrant->p(0,ii)-points[i][0])<tols[i][0] && fabs(quadrant->p(1,ii)-points[i][1])<tols[i][1] && fabs(quadrant->p(2,ii)-points[i][2])<tols[i][2]) {
            id[i] = quadrant->t(ii);
            found = true;
            std::cout << "Point " << i+1 << ": x= " << quadrant->p(0,ii) << ", y= " << quadrant->p(1,ii) << ", z= " << quadrant->p(2,ii) << std::endl;
            break;
          }
        }

        if (found)
          break;
    }
    if(!found)
        std::cout << "Node " << i << " not found in current rank" << std::endl; 
  }
  return id;
}