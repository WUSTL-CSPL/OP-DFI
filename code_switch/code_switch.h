
#ifndef CODE_SWITCH_H


#ifdef __cplusplus
extern "C" {
    void code_switching();
    void opdfi_code_switching();
    void opdfi_instrumented_code_switching(void **,void **);
  
}

#else
  void code_switching();  
  void opdfi_code_switching();
  void opdfi_instrumented_code_switching(void **, void **);
#endif

extern int slack_level;

extern int code_version_id;

#endif /* CODE_SWITCH_H */