/* Izhikevich network
 * Chen-Fu Yeh, 2019/12/04
 */

#ifndef IZ_NETWORK_H
#define IZ_NETWORK_H
#include "iz_neuron.h"
#include "weight_index_list.h"
#include <math.h>
#include <omp.h>

class iz_network
{
public:
    iz_network();
    ~iz_network();
    int num_neurons();
    void send_synapse();
    void printfile(FILE **fp_potential, FILE **fp_adaptive_term);
    void set_biascurrent(int neuron_index, float biascurrent);
    float potential(int neuron_index);
    float adaptive_term(int neuron_index);
    int spike_count(int neuron_index);
    float spike_rate(int neuron_index);
    void set_num_threads(int num_threads);

protected:  // non-copyable and non-movable
    iz_network(const iz_network& other) = delete;
    iz_network(iz_network&& other) = delete;
    iz_network& operator=(const iz_network& other) = delete;
    iz_network& operator=(iz_network&& other) = delete;

private:
    int linenum_neuronParameter();
    int set_neurons();
    int get_weight();
    int _num_neurons;
    int *_tau;
    float *_weight, *_scurrent, *_ncurrent, *_biascurrent;
    iz_neuron *_neurons;
    weight_index_list *_wlist;
    int _num_threads = 1;
};

#endif

