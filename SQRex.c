#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "svm.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

struct svm_model* model;
void swap(double *a, double *b)
{
  double tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}
void swap_idx(int *a, int *b)
{
  int tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}
int quicksort(double *arr, int *arr_idx, int left, int right)
{
  int i = left;
  int j = right;
  double key = arr[left];
  int key_idx = arr_idx[left];
  if (i >= j)
    return 0;
  while (i != j) {
  //accomplish one quicksort
    while (arr[j] > key) {
      j--;
    }
    swap(&arr[j], &key);
    swap_idx(&arr_idx[j], &key_idx);
    while (arr[i] < key) {
      i++;
    }
    swap(&arr[i], &key);
    swap_idx(&arr_idx[i], &key_idx);
  }
  quicksort(arr, arr_idx, left, i - 1); //accomplish the left remaining quicksort
  quicksort(arr, arr_idx, j + 1, right); //accomplish the right remaining quicksort
  return 1;
}

int quicksort_2(double *ratio, int *idx_select, double *threshold, double *TPR, double *FPR, int left, int right)
{
  int i = left;
  int j = right;
  double key = ratio[left];
  int key_idx = idx_select[left];
  double key_threshold = threshold[left];
  double key_TPR = TPR[left];
  double key_FPR = FPR[left];
  if (i >= j) //avoid infinity recursion
    return 0;
  while (i != j) {
  //accomplish one quicksort
    while (ratio[j] > key) {
      j--;
    }
    swap(&ratio[j], &key);
    swap_idx(&idx_select[j], &key_idx);
    swap(&threshold[j], &key_threshold);
    swap(&TPR[j], &key_TPR);
    swap(&FPR[j], &key_FPR);
    while (ratio[i] < key) {
      i++;
    }
    swap(&ratio[i], &key);
    swap_idx(&idx_select[i], &key_idx);
    swap(&threshold[i], &key_threshold);
    swap(&TPR[i], &key_TPR);
    swap(&FPR[i], &key_FPR);
  }
  quicksort_2(ratio, idx_select, threshold, TPR, FPR, left, i - 1); //accomplish the left remaining quicksort
  quicksort_2(ratio, idx_select, threshold, TPR, FPR, j + 1, right); //accomplish the right remaining quicksort
  return 1;
}



int main(int argc, char **argv)
{
model = svm_load_model(argv[1]);

double * decision_value = new double[model->l];
int * predict_label = new int[model->l];
int * predict_state = new int[model->l];//1:TP 2:FP 3:TN 4:FN
double decval = 0;
int num_TP = 0;
int num_FP = 0;
int num_TN = 0;
int num_FN = 0;
printf("Reclassify the Support Vectors\n");
for(int i = 0; i < model->l; ++i)
{
predict_label[i] = svm_predict_values(model, model->SV[i], &decval);
decision_value[i] = decval;
if(model->sv_coef[0][i] > 0 && predict_label[i] == 1){
    predict_state[i] = 1;
    num_TP ++;
}
else if(model->sv_coef[0][i] <= 0 && predict_label[i] == 1){
    predict_state[i] = 2;
    num_FP ++;
}
else if(model->sv_coef[0][i] <= 0 && predict_label[i] == -1){
    predict_state[i] = 3;
    num_TN ++;
}
else if(model->sv_coef[0][i] > 0 && predict_label[i] == -1){
    predict_state[i] = 4;
    num_FN ++;
}
}
printf("\n TP:%d FP:%d TN:%d FN:%d\n",num_TP,num_FP,num_TN,num_FN);
int num_feature = 1250;

double *mean_1 = new double[num_feature];
double *mean_2 = new double[num_feature];
double *mean_P = new double[num_feature];
double *mean_N = new double[num_feature];

int *num_total = new int[num_feature];
int *num_P = new int[num_feature];
int *num_N = new int[num_feature];

double *var = new double[num_feature];
double *sigma = new double[num_feature];

double *zscore = new double[num_feature];

for(int i = 0; i < num_feature; ++i)
{
    mean_1[i] = 0;
    mean_2[i] = 0;
    mean_P[i] = 0;
    mean_N[i] = 0;

    num_total[i] = 0;
    num_P[i] = 0;
    num_N[i] = 0;

    var[i] = 0;
    sigma [i] = 0;
    zscore[i] = 0;
    
    for(int j = 0; j < model->l; ++j)
    {
        int flag = 0;
        if(predict_state[j] == 1 || predict_state[j] == 3){
        for(int k = 0; model->SV[j][k].index != -1; k ++)
        {
            if(model->SV[j][k].index == (i + 1))
            {
            flag = 1;
            num_total[i] ++;
            mean_1[i] += model->SV[j][k].value;
            mean_2[i] += model->SV[j][k].value * model->SV[j][k].value;
            if(model->sv_coef[0][j] > 0){
                num_P[i]++;
                mean_P[i] += model->SV[j][k].value;
            }
            else{
                num_N[i]++;
                mean_N[i] += model->SV[j][k].value;
            }
            }
        } 
        if(flag == 0){
        num_total[i] ++;
        if(model->sv_coef[0][j] > 0)
            num_P[i]++;
        else
            num_N[i]++;
        }
        }
    }
}

for(int i = 0; i < num_feature; ++i)
{
    printf("%d: total:%d ",i+1,num_total[i]);
    if(num_total[i] != 0){
    mean_1[i] /= num_total[i];
    mean_2[i] /= num_total[i];
    printf("mean_1:%f mean_2:%f ",mean_1[i],mean_2[i]);
    var[i] = mean_2[i] - mean_1[i] * mean_1[i];
    sigma[i] = sqrt(var[i]);
    printf("var:%f sigma:%f ",var[i],sigma[i]);
        if(num_P[i] != 0){
        mean_P[i] /= num_P[i];
        printf("mean_P:%f ",mean_P[i]);
        }
        if(num_N[i] != 0){
        mean_N[i] /= num_N[i];
        printf("mean_N:%f ",mean_N[i]);
        }
        if(num_P[i] != 0 && num_N[i] != 0 && var[i] != 0 && sigma[i]!= 0){
        zscore[i] = (mean_P[i] - mean_N[i])/sigma[i];
        printf("zscore:%f ",zscore[i]);
        }
    }
    printf("\n");
}
double *zscore_fabs = new double[num_feature];
int num_2 = 0;
int num_3 = 0;
int num_4 = 0;
int num_5 = 0;
int num_7 = 0;
int num_10 = 0;
int j = 0;
for(int i = 0; i < num_feature; ++i){
    zscore_fabs[i] = fabs(zscore[i]);
    if(zscore_fabs[i] >= 0.2) num_2 ++;
    if(zscore_fabs[i] >= 0.3) num_3 ++;
    if(zscore_fabs[i] >= 0.4) num_4 ++;
    if(zscore_fabs[i] >= 0.5) num_5 ++;    
    if(zscore_fabs[i] >= 0.7) num_7 ++;
    if(zscore_fabs[i] >= 1.0) num_10 ++;
}
printf("Number of features whose zscore is\n");
printf(">=0.2:%d\n>=0.3:%d\n>=0.4:%d\n>=0.5:%d\n>=0.7:%d\n>=1.0:%d\n",num_2,num_3,num_4,num_5,num_7,num_10);
double *zscore_fabs_select = new double[num_10];
int *idx_select = new int[num_10];
j = 0;
for(int i = 0; i < num_feature; ++i){
    if(zscore_fabs[i] >= 1.0){
    idx_select[j] = i;
    zscore_fabs_select[j] = zscore_fabs[i];
    ++j;
    }
}

//for(int i = 0; i < num_10; ++i)
//    printf("index:%d zscore:%f total:%d P:%d N:%d mean_1:%f mean_2:%f var:%f sigma:%f mean_P:%f mean_N:%f\n",idx_select[i],zscore[idx_select[i]],num_total[idx_select[i]],num_P[idx_select[i]],num_N[idx_select[i]],mean_1[idx_select[i]],mean_2[idx_select[i]],var[idx_select[i]],sigma[idx_select[i]],mean_P[idx_select[i]],mean_N[idx_select[i]]);
printf("\n");
quicksort(zscore_fabs_select, idx_select, 0, num_10 - 1);
printf("Features whose zscore is bigger than 1.0:\n");
for(int i = num_10 - 1; i >= 0; --i)
    printf("index:%d zscore:%f mean_1:%f mean_2:%f var:%f sigma:%f mean_P:%f mean_N:%f\n",idx_select[i] + 1,zscore[idx_select[i]],mean_1[idx_select[i]],mean_2[idx_select[i]],var[idx_select[i]],sigma[idx_select[i]],mean_P[idx_select[i]],mean_N[idx_select[i]]);

double *threshold = new double[num_10];
double *TPR = new double[num_10];
double *FPR = new double[num_10];
double *ratio = new double[num_10];

printf("Rank the features by TPR/FPR ratio:\n");
for(int i = 0; i < num_10; ++i){
double threshold_1 = -1;
int TP = 0;
int FP = 0;
double ratio_1 = 0;
int index = idx_select[i];

//printf("index:%d zscore:%f ",index,zscore[index]);
if(zscore[index] > 0)
{
  for(double threshold_2 = mean_N[index]; threshold_2 <= mean_P[index] ; threshold_2 += 0.001){
    int TP_tmp = 0;
    int FP_tmp = 0;
    //ratio = 0;
    //threshold = -1;
    for(int j = 0; j < model->l ; ++j){
        int flag = 0;
        if(predict_state[j] == 1){
            for(int k = 0; model->SV[j][k].index != -1; k ++){
                if(model->SV[j][k].index == (index + 1)){
                    flag = 1;
                    if(model->SV[j][k].value > threshold_2)
                        TP_tmp++;
                }
            }
            if(flag == 0){
            if( 0 > threshold_2)
                TP_tmp ++;
            }
        }
        else if(predict_state[j] == 3){
            for(int k = 0; model->SV[j][k].index != -1; k ++){
                if(model->SV[j][k].index == (index + 1)){
                    flag = 1;
                    if(model->SV[j][k].value > threshold_2)
                        FP_tmp ++;
                }
            }
            if(flag == 0){
            if( 0 > threshold_2)
                FP_tmp ++;
            }
        }
    }
    //printf("threshold2:%f TP_tmp:%d FP_tmp:%d ratio_tmp:%f ",threshold_2,TP_tmp,FP_tmp,TP_tmp/FP_tmp);
    if(ratio_1 < (TP_tmp * 1.0 / FP_tmp)){
    TP = TP_tmp;
    FP = FP_tmp;
    ratio_1 = TP * 1.0 / FP;
    threshold_1 = threshold_2;
    }
    //printf("threshold1:%f TP:%d FP:%d ratio:%f \n",threshold_1,TP,FP,ratio_1);
  }//for
}//if
else
{
  for(double threshold_2 = mean_N[index]; threshold_2 >= mean_P[index] ; threshold_2 -= 0.001){
    int TP_tmp = 0;
    int FP_tmp = 0;
    //ratio = 0;
    //threshold = -1;
    for(int j = 0; j < model->l ; ++j){
        int flag = 0;
        if(predict_state[j] == 1){
            for(int k = 0; model->SV[j][k].index != -1; k ++){
                if(model->SV[j][k].index == (index + 1)){
                    flag = 1;
                    if(model->SV[j][k].value < threshold_2)
                        TP_tmp++;
                }
            }
            if(flag == 0){
            if( 0 < threshold_2)
                TP_tmp ++;
            }
        }
        else if(predict_state[j] == 3){
            for(int k = 0; model->SV[j][k].index != -1; k ++){
                if(model->SV[j][k].index == (index + 1)){
                    flag = 1;
                    if(model->SV[j][k].value < threshold_2)
                        FP_tmp ++;
                }
            }
            if(flag == 0){
            if( 0 < threshold_2)
                FP_tmp ++;
            }
        }
    }
    //printf("threshold2:%f TP_tmp:%d FP_tmp:%d ratio_tmp:%f ",threshold_2,TP_tmp,FP_tmp,TP_tmp * 1.0 /FP_tmp);
    if(ratio_1 < (TP_tmp * 1.0 / FP_tmp)){
    TP = TP_tmp;
    FP = FP_tmp;
    ratio_1 = TP * 1.0 / FP;
    threshold_1 = threshold_2;
    }
    //printf("threshold1:%f TP:%d FP:%d ratio:%f \n",threshold_1,TP,FP,ratio_1);
  }//for
}//else
//printf("TP:%d FP:%d ",TP,FP);
threshold[i] = threshold_1;
TPR[i] = TP * 1.0 / num_TP;
FPR[i] = FP * 1.0 / num_TN;
ratio[i] = ratio_1 * num_TN / num_TP;
//printf("threshold:%f TPR:%f FPR:%f ratio:%f \n",threshold[i],TPR[i],FPR[i],ratio[i]);
}//for
quicksort_2(ratio, idx_select, threshold, TPR, FPR, 0, num_10 - 1);
for(int i = num_10 - 1; i >= 0; --i){
    printf(" |   %d   |  %f  | %f | %f | %f | %f |\n",idx_select[i] + 1,zscore[idx_select[i]],threshold[i],TPR[i],FPR[i],ratio[i]);
    printf(" +---------+------------+-----------+----------+----------+-----------+\n");
}

int **state_P = new int*[num_10];
for(int i = 0; i < num_10; ++i){
    state_P[i] = new int[num_TP];
    memset(state_P[i], 0, sizeof(** state_P) * (num_TP));
}

int **state_N = new int*[num_10];
for(int i = 0; i < num_10; ++i){
    state_N[i] = new int[num_TN];
    memset(state_N[i], 0, sizeof(** state_N) * (num_TN));
}

for(int i = 0; i < num_10 ; ++i){
int count_P = 0;
int count_N = 0;
int index = idx_select[i];
if(zscore[index] > 0)
{
    for(int j = 0; j < model->l ; ++j){
        int flag = 0;
        if(predict_state[j] == 1){
            for(int k = 0; model->SV[j][k].index != -1; k ++){
                if(model->SV[j][k].index == (index + 1)){
                    flag = 1;
                    if(model->SV[j][k].value > threshold[i])
                        state_P[i][count_P] = 1;
                }
            }
            if(flag == 0){
            if( 0 > threshold[i])
                state_P[i][count_P] = 1;
            }
            count_P ++;
        }
        else if(predict_state[j] == 3){
            for(int k = 0; model->SV[j][k].index != -1; k ++){
                if(model->SV[j][k].index == (index + 1)){
                    flag = 1;
                    if(model->SV[j][k].value > threshold[i])
                        state_N[i][count_N] = 1;
                }
            }
            if(flag == 0){
            if( 0 > threshold[i])
                state_N[i][count_N] = 1;
            }
            count_N ++;
        }
    }
    
}//if
else
{
    for(int j = 0; j < model->l ; ++j){
        int flag = 0;
        if(predict_state[j] == 1){
            for(int k = 0; model->SV[j][k].index != -1; k ++){
                if(model->SV[j][k].index == (index + 1)){
                    flag = 1;
                    if(model->SV[j][k].value < threshold[i])
                        state_P[i][count_P] = 1;
                }
            }
            if(flag == 0){
            if( 0 < threshold[1])
                state_P[i][count_P] = 1;
            }
            count_P ++;
        }
        else if(predict_state[j] == 3){
            for(int k = 0; model->SV[j][k].index != -1; k ++){
                if(model->SV[j][k].index == (index + 1)){
                    flag = 1;
                    if(model->SV[j][k].value < threshold[i])
                        state_N[i][count_N] = 1;
                }
            }
            if(flag == 0){
            if( 0 < threshold[i])
                state_N[i][count_N] = 1;
            }
            count_N ++;
        }
    }
}//else
}//for
printf("\n");
/*
for(int i = 0; i < num_10; ++i){
    printf("index:%d ",idx_select[i]);
    double sum_TP = 0;
    double sum_FP = 0;
    for(int j = 0; j < num_TP; ++j)
        sum_TP += state_P[i][j];
    printf("TPR_1:%f TPR:%f ",sum_TP * 1.0 / num_TP, TPR[i]);
    for(int j = 0; j < num_TN; ++j)
        sum_FP += state_N[i][j];
    printf("FPR_1:%f FPR:%f ",sum_FP * 1.0 / num_TN, FPR[i]);
}
*/

int *sum_state_P = new int[num_TP];
memset(sum_state_P, 0, sizeof(*sum_state_P) * (num_TP));

for(int i = 0; i < num_TP; ++i){
    for(int j = 0; j < num_10; ++j){
    //if(ratio[j] >= 10)
    sum_state_P[i] += state_P[j][i];
    }
}
int sum_TP = 0;
for(int i = 0; i < num_TP; ++i){
    if(sum_state_P[i] >= 1) sum_state_P[i] = 1;
    sum_TP += sum_state_P[i];
}

int *sum_state_N = new int[num_TN];
memset(sum_state_N, 0, sizeof(*sum_state_N) * (num_TN));

for(int i = 0; i < num_TN; ++i){
    for(int j = 0; j < num_10; ++j){
    //if(ratio[j] >= 10)
    sum_state_N[i] += state_N[j][i];
    }
}
int sum_FP = 0;
for(int i = 0; i < num_TN; ++i){
    if(sum_state_N[i] >= 1) sum_state_N[i] = 1;
    sum_FP += sum_state_N[i];
}

printf("total TP:%d total FP:%d \n", sum_TP, sum_FP);

int *rule_state_P = new int[num_TP];
memset(rule_state_P, 0, sizeof(*rule_state_P) * (num_TP));
int *rule_state_N = new int[num_TN];
memset(rule_state_N, 0, sizeof(*rule_state_N) * (num_TN));

int *rule_state = new int[num_10];
memset(rule_state, 0, sizeof(*rule_state) * (num_10));
rule_state[21] = 1;
rule_state[20] = 1;
rule_state[19] = 1;
rule_state[18] = 1;
rule_state[16] = 1;
rule_state[15] = 1;
rule_state[14] = 1;
rule_state[10] = 1;

int sum_TP_pre = 0;
int sum_TP_new = 0;
int sum_FP_pre = 0;
int sum_FP_new = 0;
for(int i = num_10 - 1; i >= 0; --i){
if(rule_state[i] == 1){
    sum_TP_new = 0;
    for(int j = 0; j < num_TP; ++j){
        rule_state_P[j] += state_P[i][j];
    }
    for(int j = 0; j < num_TP; ++j){
    if(rule_state_P[j] >= 1) rule_state_P[j] = 1;
    sum_TP_new += rule_state_P[j];
    }

    sum_FP_new = 0;
    for(int j = 0; j < num_TN; ++j){
        rule_state_N[j] += state_N[i][j];
    }
    for(int j = 0; j < num_TN; ++j){
    if(rule_state_N[j] >= 1) rule_state_N[j] = 1;
    sum_FP_new += rule_state_N[j];
    }
    //printf("index:%d new TP:%d new FP:%d \n",idx_select[i],sum_TP_new - sum_TP_pre, sum_FP_new - sum_FP_pre);
    sum_TP_pre = sum_TP_new;
    sum_FP_pre = sum_FP_new;
}
}
int rule_TP = sum_TP_new;
int rule_FP = sum_FP_new;
double rule_TPR = rule_TP * 1.0 / num_TP;
double rule_FPR = rule_FP * 1.0 / num_TN;
double rule_ratio = rule_TPR / rule_FPR ;


struct rule{
int index;
double threshold;
int state;
};
int num_rule = 0;
for(int i = 0; i < num_10; i ++)
    num_rule += rule_state[i];
struct rule *rule_set = new struct rule[num_rule];
int count_rule = 0;
for(int i = 0; i < num_10; i ++){
    if(rule_state[i] == 1){
    rule_set[count_rule].index = idx_select[i] + 1;
    rule_set[count_rule].threshold = threshold[i];
    rule_set[count_rule].state = zscore[idx_select[i]] > 0?1:-1;
    count_rule++;
    }
}
printf("Rule Set:\n");
for(int i = 0; i < num_rule; i ++){
printf("%d %f %d \n",rule_set[i].index,rule_set[i].threshold,rule_set[i].state);
}
printf("TP:%d FP:%d TPR:%f FPR:%f ratio:%f \n",rule_TP,rule_FP,rule_TPR,rule_FPR,rule_ratio);

}//end of main
