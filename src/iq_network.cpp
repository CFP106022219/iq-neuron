#include "iq_network.h"

using namespace std;

iq_network::iq_network()
{
    _num_neurons = linenum_neuronParameter();
    _neurons = new iq_neuron[_num_neurons];
    _tau = new int[_num_neurons * _num_neurons]();
    _f = new int[_num_neurons * _num_neurons]();
    _n = new int[_num_neurons * _num_neurons]();
    _weight = new int[_num_neurons * _num_neurons]();
    _current = new int[_num_neurons * _num_neurons]();
    _biascurrent = new int[_num_neurons]();

    get_weight();
    set_neurons();
    return;
}

iq_network::~iq_network()
{
    delete[] _weight;
    delete[] _current;
    delete[] _biascurrent;
    delete[] _tau;
    delete[] _f;
    delete[] _n;
    delete[] _neurons;
    return;
}

int iq_network::linenum_neuronParameter()
{
    int i[7], linenum = 0;
    FILE *fp = fopen("../inputs/neuronParameter_IQIF.txt", "r");
    if(fp == NULL) {
        printf("neuronParameter_IQIF.txt file not opened\n");
        return -1;
    }
    
    while(fscanf(fp, " %d %d %d %d %d %d %d", &i[0], &i[1], &i[2],
            &i[3], &i[4], &i[5], &i[6]) == 7) {
        linenum++;
    }
    fclose(fp);
    return linenum;
}

int iq_network::set_neurons()
{
    int i, rest, threshold, reset, a, b, noise;
    FILE *fp;

    /*
    if(*(_tau + 0) == 0) {
        printf("synapse decay time constant _tau = %d\n", _tau);
        printf("error: please get_weight() first to set _tau.\n");
        return 1;
    }
    */
    fp = fopen("../inputs/neuronParameter_IQIF.txt", "r");
    while(fscanf(fp, " %d %d %d %d %d %d %d", &i, &rest, &threshold,
            &reset, &a, &b, &noise) == 7) {
        (_neurons + i)->set(rest, threshold, reset, a, b, noise);
    }
    fclose(fp);
    return 0;
}

int iq_network::get_weight()
{
    int i, j, temp, temptwo;
    FILE *fp;
    for(i = 0; i < _num_neurons; i++) {
        for(j = 0; j < _num_neurons; j++) {
            *(_current + _num_neurons*i + j) = 0;
            *(_weight + _num_neurons*i + j) = 0;
            *(_tau + _num_neurons*i + j) = 0;
            *(_f + _num_neurons*i + j) = 0;
            *(_n + _num_neurons*i + j) = 0;
        }
        *(_biascurrent + i) = 0;
    }

    fp = fopen("../inputs/Connection_Table_IQIF.txt", "r");
    if(fp == NULL) {
        printf("Connection_Table_IQIF.txt file not opened\n");
        return 1;
    }

    while(fscanf(fp, "%d %d %d %d", &i, &j, &temp, &temptwo) == 4) {
        *(_weight + _num_neurons*i + j) = temp;
        *(_tau + _num_neurons*i + j) = temptwo;
        if(temptwo >= 10) {
            *(_f + _num_neurons*i + j) = (int) (log10(0.9) / log10((temptwo-1)/(float) temptwo));
            printf("tau[%d][%d] = %d\n", i, j, *(_tau + _num_neurons*i + j));
        }
        else {
            printf("error: synapse time constant cannot be less than 10!\n");
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int iq_network::num_neurons()
{
    return _num_neurons;
}

void iq_network::send_synapse()
{
    int i, j, total_current;

    /* accumulate individual synapse current */
    for(i = 0; i < _num_neurons; i++) {
        if((_neurons + i)->is_firing()) {
            //printf("neuron %d has fired!\n", i);
            for(j = 0; j < _num_neurons; j++) {
                *(_current + _num_neurons*i + j) += *(_weight + _num_neurons*i + j);
            }
        }
    }

    /* accumulate and inject current into neurons, solving DE */
    for(j = 0; j < _num_neurons; j++) {
        total_current = 0;
        for(i = 0; i < _num_neurons; i++) {
            total_current += *(_current + _num_neurons*i + j);
        }
        (_neurons + j)->iq(total_current + *(_biascurrent + j));
    }

    /* synapse exponential decay */
    for(i = 0; i < _num_neurons; i++) {
        for(j = 0; j < _num_neurons; j++) {
            if(*(_n + _num_neurons*i + j) > *(_f + _num_neurons*i + j)) {
                *(_n + _num_neurons*i + j) = 0;
                *(_current + _num_neurons*i + j) = *(_current + _num_neurons*i + j) * 9 / 10;
            }
            *(_n + _num_neurons*i + j) += 1;
        }
    }

    return;
}

void iq_network::printfile(FILE **fp)
{
    int i;

    for(i = 0; i < _num_neurons; i++) {
        fprintf(fp[i], "%d\n", (_neurons+i)->potential());
    }
    return;
}

void iq_network::set_biascurrent(int neuron_index, int biascurrent)
{
    _biascurrent[neuron_index] = biascurrent;
    return;
}


