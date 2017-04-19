
#include <cmath>
#include <vector>
#include <iostream>

#include <boost/array.hpp>
#include <boost/numeric/odeint.hpp>
using namespace boost::numeric::odeint;

#include <boost/random/normal_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

const float w_radial =  2.0e6;
const float w_axial = 0.7e6;

const float IoIsat = 1.;
const float delta = -50e6;
const float Gamma = 100e6;
const float k_493 = 2 * 3.141592654 / 493.e-9;
const float k_vec[3] = { 0.5, 0.5, 1/sqrt(2.) };
const float hbar = 6.62606957e-34;

const float m_Ba = 138 * 1.660538921e-27;
const float m_Yb = 174 * 1.660538921e-27;
const float species_masses[2] = { m_Ba, m_Yb };

const float k_E = 9e9;
const float e = 1.602176565e-19; 

size_t nions = 7;
bool output_trace = false;
float beta = 0;

typedef std::vector< size_t > ion_species_type;
typedef std::vector< float > state_type;

template< typename base_state_type  > class stochastic_euler
{
public:

    typedef base_state_type state_type;
    typedef base_state_type deriv_type;
    typedef double value_type;
    typedef double time_type;
    typedef unsigned short order_type;
    typedef boost::numeric::odeint::stepper_tag stepper_category;

    static order_type order( void ) { return 1; }

	template< class System >
	void do_step( System system , state_type &x , 
		time_type t , time_type dt ) const
    {
        deriv_type det( x.size() ), stoch( x.size() ) ;
        system.first( x, det, t  );
        system.second( x, stoch, t );
        for( size_t i=0 ; i<x.size() ; ++i )
            x[i] += dt * det[i] + sqrt( dt ) * stoch[i];
    }
};

struct IonStepper {
	const ion_species_type& species;
	const double cooling_start;

	IonStepper( const ion_species_type& species_, const double cool_ = 0. ): 
		species( species_ ), cooling_start( cool_ )
	{}

	void operator()( const state_type &ions, state_type &dions,
		const double t = 0. ) const {

		
		if( output_trace ) std::cout << t << "\t";
		for( size_t i = 0 ; i < nions; ++i ) {
			const float* ion = &ions[ i*6 ];
			float* dion = &dions[ i*6 ];

			size_t s = species[i];
			float mass = species_masses[s];

			if( output_trace ) std::cout << ion[2] << "\t";

			dion[0] = ion[3];
			dion[1] = ion[4];
			dion[2] = ion[5];

			float mscale = species_masses[0] / mass;
			dion[3] = -mscale*mscale*w_radial*w_radial * ion[0] - beta * ion[3];
			dion[4] = -mscale*mscale*w_radial*w_radial * ion[1] - beta * ion[4];
			dion[5] = -mscale*w_axial*w_axial   * ion[2] - beta * ion[5];

			if( s == 0 && t >= cooling_start ) {
				float kv = k_vec[0]*ion[3] + k_vec[1]*ion[4] + k_vec[2]*ion[5];
				float scatter = Gamma / 2 * IoIsat / 
					(1 + IoIsat + 4*(delta + k_493*kv)*(delta + k_493*kv)
					/Gamma / Gamma);

				dion[3] += - hbar*k_vec[0]*k_493*scatter / mass;
				dion[4] += - hbar*k_vec[1]*k_493*scatter / mass;
				dion[5] += - hbar*k_vec[2]*k_493*scatter / mass;
			}

			for( size_t j = 0; j < nions; ++j ) {
				if( i == j ) continue;
				float r = sqrt( 
					( ion[0] - ions[j*6 + 0] )*( ion[0] - ions[j*6 + 0] ) +
					( ion[1] - ions[j*6 + 1] )*( ion[1] - ions[j*6 + 1] ) +
					( ion[2] - ions[j*6 + 2] )*( ion[2] - ions[j*6 + 2] ) );

				float F = k_E * e * e / r / r / r / mass;
				dion[3] += F * ( ion[0] - ions[j*6 + 0] );
				dion[4] += F * ( ion[1] - ions[j*6 + 1] );
				dion[5] += F * ( ion[2] - ions[j*6 + 2] );
			}
		}

		if( output_trace ) std::cout << std::endl;
	}
};

struct IonNoiseStepper {
	boost::mt19937 rng;
    boost::normal_distribution<> dist;

	IonNoiseStepper( double sigma ): rng(), dist( 0., sigma )
	{}

	void operator()( const state_type& ions, state_type& dions,
		const float t = 0. ) {

		for( size_t i = 0 ; i < nions; ++i ) {
			const float* ion = &ions[ i*6 ];
			float* dion = &dions[ i*6 ];
			dion[0] = dion[1] = dion[2] = 0.;
			dion[3] = dist( rng ) * e / m_Ba;
			dion[4] = dist( rng ) * e / m_Ba;
			dion[5] = dist( rng ) * e / m_Ba;
		}
	} 

};

class IonSimulator {
	size_t nions, ndark;
	ion_species_type species;
	state_type initial;

	void initialize() {
		initial.resize( nions*6 );
		for( size_t i = 0; i < nions; ++i ) {
			initial[ i*6 + 0 ] = 0.0;
			initial[ i*6 + 1 ] = 0.0;
			initial[ i*6 + 2 ] = 15e-6*i - 15e-6*nions / 2;
			initial[ i*6 + 3 ] = 0.0;
			initial[ i*6 + 4 ] = 0.0;
			initial[ i*6 + 5 ] = 0.0;
		}

		beta = 1e6;
		typedef runge_kutta_cash_karp54< state_type > error_stepper_type;
		integrate_adaptive( make_controlled( 1.0e-8, 1.0e-6, 
				error_stepper_type()), 
			IonStepper( species ), initial, 0.0, 1.e-4, 1.e-8 );
		sort_ions( initial );
		beta = 0.;
	}

	void sort_ions( state_type& initial ) {
		for( size_t i = 0; i < nions; ++i ) {
			for( size_t j = 0; j < nions-1; ++j ) {
				if( initial[ (j+1)*6 + 2 ] < initial[ j*6 + 2 ] ) {
					std::swap( initial[ (j+1)*6 + 0 ], initial[ j*6 + 0 ] );
					std::swap( initial[ (j+1)*6 + 1 ], initial[ j*6 + 1 ] );
					std::swap( initial[ (j+1)*6 + 2 ], initial[ j*6 + 2 ] );
					std::swap( species[ j+1 ], species[ j ] );
				}
			}
		}
	}

public:
	IonSimulator( const size_t nions_, const size_t ndark_ ): 
		nions( nions_ ), ndark( ndark_ )
	{
		srand(time(0));
		species.resize( nions );
		for( size_t i = 0; i < ndark; ++i )
			species[i] = 1;
	}

	void run_dark( const size_t nruns, const size_t dark_time_ms ) {
		const double dt = 1e-10;//1e-8;

		// SE ~ 1e-6
		const double sigma = sqrt( 1e-6 / w_axial );
		initialize();
	
		output_trace = true;
		integrate_const( stochastic_euler< state_type >(),
			std::make_pair( IonStepper( species, dark_time_ms*1e-3 ), 
				IonNoiseStepper( sigma ) ),
			initial, 0.0, (dark_time_ms + 1)*1e-3, dt );
	}

	float run_velocity( const size_t nruns, const float v, const float dark ) {
		size_t swaps = 0;

		for( size_t r = 0; r < nruns; ++r ) {
			//Leave me in for real data
			//std::random_shuffle( species.begin(), species.end() );
			initialize();

			state_type x( nions*6 );
			for( size_t i = 0; i < nions; ++i ) {
				x[ i*6 + 0 ] = initial[ i*6 + 0 ];
				x[ i*6 + 1 ] = initial[ i*6 + 1 ];
				x[ i*6 + 2 ] = initial[ i*6 + 2 ];

				float vx = (float)rand() / RAND_MAX - 0.5;
				float vy = (float)rand() / RAND_MAX - 0.5;
				float vz = (float)rand() / RAND_MAX - 0.5;
				float vmag = sqrt( vx*vx + vy*vy + vz*vz );
				x[ i*6 + 3 ] = v * vx / vmag;
				x[ i*6 + 4 ] = v * vy / vmag;
				x[ i*6 + 5 ] = v * vz / vmag;
			}

			typedef runge_kutta_cash_karp54< state_type > error_stepper_type;
			integrate_adaptive( make_controlled( 1.0e-8, 1.0e-6, 
					error_stepper_type()),
				IonStepper( species, dark ), x, 0.0, dark + 1.e-4, 1.e-8 );

			std::vector< size_t > initial_species( species );
			sort_ions( x );
	
			for( size_t j = 0; j < nions; ++j ) {
				std::cout << species[j];
				if( species[j] != initial_species[j] ) {
					++swaps;
					//break; //Leave me in for real data
				}
			}
			std::cout << std::endl;
		}
		return (float)swaps / nruns;
	}

};

int main( int argc, char** argv ) {
	if( argc < 3 ) {
		std::cout << "usage: " << argv[0] << " <nions> <ndark>" << std::endl;
		exit(0);
	}

	nions = atoi( argv[1] );
	size_t ndark = atoi( argv[2] );
	if( ndark > nions ) {
		std::cout << "<nions> must be >= <ndark>" << std::endl;
		exit(0);
	}

	IonSimulator sim( nions, ndark );

	for( size_t i = 0; i <= 40; ++i ) {
		float v = (float)i / 4 + 5;
		float prob = sim.run_velocity( 60/*10000*/, v, 1e-4 );
		std::cout << v << "\t" << prob << std::endl;
	}

	sim.run_dark( 10000, 1 );

	return 0;
}

