#include "../common.h"
#include <llvm/Analysis/CallGraph.h>
#include <iostream>
#include <algorithm>
#define TRUE 1
#define FALSE 0

std::string slip(std::string base,int begin,int end){ // exclude end 
  std::string temp="";
  for(int i=begin;i<end;i++){
    temp += base[i];
  }
  return temp;
}
void stringSetSlip(std::set<std::string> &errHandlerList,std::set<std::string>& sliceSet){
  for(std::string s : errHandlerList){
    int sflag = 0;
    for(unsigned i=0;i<s.size();i++){
      if(i==0) {
        if(s[i]=='_') sflag = i+1;
        continue;
      }
      if(i==s.size()-1) {
        if(s[i]=='_') sliceSet.insert(slip(s,sflag,i));
        else sliceSet.insert(slip(s,sflag,i+1));
        break;
      }
      if(s[i]=='_'){ //(s[i-1]>=65 && s[i-1] <= 90 || s[i-1] << 122 &&  s[i-1] >=97 || s[i-1] >=48 && s[i-1] <=57) &&
        if(s[i-1]=='_') {sflag = i+1;}
        else{
          sliceSet.insert(slip(s,sflag,i));
          sflag = i+1;
        }
      }
      
    }
  }
}

bool subStrMatch(std::string longstr,std::string shootstr){
  std::set<std::string> sliceSet;
  std::string s=longstr;
  int sflag = 0;
  for(unsigned i=0;i<s.size();i++){
    if(i==0) {
      if(s[i]=='_') sflag = i+1;
      continue;
    }
    if(i==s.size()-1) {
      if(s[i]=='_') sliceSet.insert(slip(s,sflag,i));
      else sliceSet.insert(slip(s,sflag,i+1));
      break;
    }
    if(s[i]=='_'){ //(s[i-1]>=65 && s[i-1] <= 90 || s[i-1] << 122 &&  s[i-1] >=97 || s[i-1] >=48 && s[i-1] <=57) &&
      if(s[i-1]=='_') {sflag = i+1;}
      else{
        sliceSet.insert(slip(s,sflag,i));
        sflag = i+1;
      }
    }
      
  }
  if(sliceSet.count(shootstr)==1)return 1;
  else return 0;
}

typedef std::pair<std::string, int> PAIR;
 
struct CmpByValue {
  bool operator()(const PAIR& lhs, const PAIR& rhs) {
    return lhs.second > rhs.second;
  }
};

void sortMapByValue(std::map<std::string, int>& tMap,std::vector<PAIR>& tVector)  {  
  for (std::map<std::string, int>::iterator curr = tMap.begin(); curr != tMap.end(); curr++)   
    tVector.push_back(make_pair(curr->first, curr->second));    
   
  std::sort(tVector.begin(), tVector.end(),CmpByValue());  
} 

void dumpslipceStatisticVector(std::vector<std::pair<std::string,int>> slipceStatisticVector,int sum){
  std::string outputFile = "slipceStatisticVector.txt";
  std::string dumpString="";
  for(auto it : slipceStatisticVector){
      dumpString += it.first+" : "+ std::to_string(it.second*1.0/sum)+"\n";
  }

  std::ofstream os;
  os.open(outputFile);
  if (os.is_open()) {
    os<<dumpString<<"\n";
    os.close();
  }
}

void basic_call_graph(Module &M){

  std::set<std::string> errHandlerConfirmList;
  std::set<std::string> errHandlerSlice;
  std::map<std::string,int> slipceStatistic;
  std::vector<std::pair<std::string,int>> slipceStatisticVector;
  
  // load the suspiciousList
  loadStringSet(errHandlerConfirmList,"errHandlerConfirmList.txt");

  // get slice from confirm list
  stringSetSlip(errHandlerConfirmList,errHandlerSlice);
  for(std::string tmpslice : errHandlerSlice){
    for(std::string tmpErrhandler : errHandlerConfirmList){
      if(subStrMatch(tmpErrhandler,tmpslice)){// 子串匹配
        if(slipceStatistic.count(tmpslice)==1) slipceStatistic[tmpslice]+=1;// map insert Operation
        else slipceStatistic[tmpslice]=1;
      }
    }
  }

  // map sort
  sortMapByValue(slipceStatistic,slipceStatisticVector);

  // dump slipceStatistic
  dumpslipceStatisticVector(slipceStatisticVector,errHandlerConfirmList.size());
  std::cout<<"total slipceStatisticVector: "<<slipceStatisticVector.size()<<std::endl;
};

class NewCallGraphPass : public ModulePass {
public:
  static char ID;
  NewCallGraphPass() : ModulePass(ID) {}
  virtual bool runOnModule (Module &M) override {
    basic_call_graph(M);
    return false;
  }
};

char NewCallGraphPass::ID = 0;

static RegisterPass<NewCallGraphPass> X("newcg", "New Call Graph Pass", false, false);
