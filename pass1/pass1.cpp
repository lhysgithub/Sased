#include "../common.h"
#include <llvm/Analysis/CallGraph.h>
#include <iostream>
#include <deque>
#include <map>
#include <set>
#include <stack>
#include <fstream>
#include <algorithm>
#include <sstream>
#define TRUE 1
#define FALSE 0
typedef std::pair<std::string, double> PAIR;

void detectSuspiciousByName(Function& F,std::set<std::string>& suspiciousList,std::set<std::string> errHandleKeywords){

  std::string FuncName = F.getName().str();

  for (std::string errandleKeyword : errHandleKeywords){
    std::string temp = errandleKeyword;

    // 检测函数名是否包含小写异常字符串集元素
    if (FuncName.find(temp)!=std::string::npos){
      if(temp == "err"){
        if(detectErrByName(FuncName)){
          suspiciousList.insert(FuncName);
          break;
        }
      }
      else{
        suspiciousList.insert(FuncName);
        break;
      }
    }

    // 检测函数名是否包含大写异常字符串集元素
    transform(temp.begin(),temp.end(),temp.begin(),::toupper);
    if (FuncName.find(temp)!=std::string::npos){
      if(temp == "ERR"){
        if(detectErrByName(FuncName)){
          suspiciousList.insert(FuncName);
          break;
        }
      }
      else{
        suspiciousList.insert(FuncName);
        break;
      }
    }
  }
  
  if (suspiciousList.count(FuncName)) return;
  else{// if function name is not including error key words, detect the arg of function.
    std::vector<ARG_PAIR> farg_pair = getFunctionArgTypeAndNameList(F);
    for(auto funcarg : farg_pair){
      if(detectSusNameByKeyWord(funcarg.second,errHandleKeywords)){
        suspiciousList.insert(FuncName);
      }
    }
  }
  
}

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c){
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));
 
    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
}

//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）
template <class Type>
Type stringToNum(const std::string& str)
{
  
  std::istringstream iss(str);
  Type num;
  iss >> num;
  return num;    
}

void loadPairVector(std::vector<PAIR>& PAIRVector,std::string loadFile){ 
  std::string temp;
  std::ifstream os(loadFile);
  while(getline(os,temp)){
    if(temp=="")break;
  	std::vector<std::string> v;
  	SplitString(temp,v," : ");
    // std::cout<<v[0]<<" : "<<v[1]<<"\n";
  	PAIRVector.push_back(make_pair(v[0],stringToNum<double>(v[1])));
	}
  // }
}

void MoudlePass1(Module &M) {
  
  std::set<std::string> suspiciousList;
  std::set<std::string> errHandleKeywords;
  std::set<std::string> negativeSlice; // human intelligence
  std::vector<PAIR> slipceStatisticVector;
  loadStringSet(errHandleKeywords,"errHandleKeywords.txt");
  loadStringSet(negativeSlice,"negativeSlice.txt");
  loadPairVector(slipceStatisticVector,"slipceStatisticVector.txt");
  double theta = 0;
  int totalF=0;

  for(auto slipcePair : slipceStatisticVector){
    // std::cout<<slipcePair.first<<" : "<<slipcePair.second<<"\n";
  	if(slipcePair.second < theta) continue; // 低于阈值
  	// std::cout<<"It's possible:"<<"\n";
    if(negativeSlice.count(slipcePair.first)==1) errHandleKeywords.insert(slipcePair.first);
  }

  // std::cout<<"negativeSlice:"<<"\n";
  // for(auto negativeSlic:negativeSlice){
  //   std::cout<<negativeSlic<<"::"<<negativeSlic.size()<<"\n";
  // }

  // std::cout<<"errHandleKeyword:"<<"\n";
  // for(auto errHandleKeyword:errHandleKeywords){
  //   std::cout<<errHandleKeyword<<"::"<<errHandleKeyword.size()<<"\n";
  // }

  for (Function &F : M){
    totalF++;
    detectSuspiciousByName(F,suspiciousList,errHandleKeywords);
  }

  dumpStringSet(suspiciousList,"suspiciousList.txt");
  std::cout<<"total suspiciousList: "<<suspiciousList.size()<<std::endl;

  dumpStringSet(errHandleKeywords,"errHandleKeywords.txt");
  std::cout<<"total errHandleKeywords: "<<errHandleKeywords.size()<<std::endl;
  
  std::cout<<"total totalF: "<<totalF<<std::endl;
}


class NewCallGraphPass : public ModulePass {
public:
  static char ID;
  NewCallGraphPass() : ModulePass(ID) {}
  virtual bool runOnModule (Module &M) override {
    MoudlePass1(M);
    return false;
  }
};

char NewCallGraphPass::ID = 0;


static RegisterPass<NewCallGraphPass> X("newcg", "New Call Graph Pass", false, false);
