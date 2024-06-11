
#ifndef ONLINE_ESTIMATOR_H


#ifdef __cplusplus
extern "C" {
    // void slack_estimate(int a, int b, int has_context=1);
    void slack_estimate(int* inputs, int num);
    void v0_slack_estimate(int* inputs, int num);
    void init();
    void reset_iteration();
    void start_iteration();
    void opdfi_slack_estimate();
    void opdfi_obtain_input(int* inputs,int num);
    int opdfi_read_input(int id)  ;
    int init_slack_estimation();
    void save_final_result();
    
}

#else
  void slack_estimate(int* inputs, int num);
  void v0_slack_estimate(int* inputs, int num);
    void init();  
     void reset_iteration();  
     void start_iteration();
     void opdfi_slack_estimate();
     void opdfi_obtain_input(int* inputs,int num);
     int opdfi_read_input(int id)  ;
     int init_slack_estimation();
     void save_final_result();
#endif


#endif /* ONLINE_ESTIMATOR_H */