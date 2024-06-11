
clang++ -c  $opdfi/slack_estimation/online_estimator.cpp -o $opdfi/slack_estimation/online_estimator.o
clang++ -c -flto $opdfi/slack_estimation/online_estimator.cpp -o $opdfi/slack_estimation/online_estimator.bc