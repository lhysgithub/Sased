#include "../common.h"
#include <llvm/Analysis/CallGraph.h>
#include <llvm/IR/CFG.h>
#include <iostream>
#include <algorithm>
#define TRUE 1
#define FALSE 0
#define DEBUG 1

typedef std::pair<std::string, std::string> String_Pair;

class CallingPath{
public:
  std::string callee;
  std::string caller;
  std::string module;
  CallingPath(std::string a,std::string b,std::string c):callee(a),caller(b),module(c){}
  bool operator <(const CallingPath c) const  //运算符重载 
  {
    if (callee!=c.callee){
      return callee<c.callee;
    }
    else if(caller!=c.caller){
      return caller<c.caller;
    }
    else return module<c.module;
  }
};

int dumpUsingTimes(std::map<std::string,int>& dumpStringList,std::string outputFile){
  std::string dumpString="";
  char buf[50];
  for(auto it : dumpStringList){
      sprintf(buf,"%d",it.second);
      dumpString += it.first+" : "+buf+"\n";
  }

  std::ofstream os;
  os.open(outputFile);
  if (os.is_open()) {
    os<<dumpString<<"\n";
    os.close();
  }
  return 1;
}


int dumpCallPath(std::set<CallingPath>& dumpStringList,std::string outputFile){
  std::string dumpString="";
  for(auto it : dumpStringList){
      dumpString += it.callee+"<-"+it.caller+"<-"+it.module+"\n";
  }

  std::ofstream os;
  os.open(outputFile);
  if (os.is_open()) {
    os<<dumpString<<"\n";
    os.close();
  }
  return 1;
}

int dumpStringPairSet(std::set<String_Pair>& dumpStringList,std::string outputFile){
  std::string dumpString="";
  for(auto it : dumpStringList){
      dumpString += it.first+"<-"+it.second+"\n";
  }

  std::ofstream os;
  os.open(outputFile);
  if (os.is_open()) {
    os<<dumpString<<"\n";
    os.close();
  }
  return 1;
}

void RecursiveConfirm(CallInst *call_inst,std::set<std::string>& suspiciousList,std::set<std::string>& errHandlerConfirmList,std::set<CallingPath>& errConfirmPath, std::map<std::string, int>& errHandlerUsedTimes){
  unsigned partBranchErrFlag = 0;

  for(BasicBlock* PBB_ptr : predecessors(call_inst->getParent())){
    // get the parent termiator instruction
    Instruction * PI = PBB_ptr->getTerminator();
    // 父块为分支结尾时
    BranchInst *PIB = dyn_cast<BranchInst>(PI);                
    if (PIB){//如果父块尾指令为branch语句
      // if(DEBUG == 1) std::cout<<"Transform BranchInst complete"<<std::endl;
      if(PIB->isConditional()){//如果父块尾指令为条件跳转语句，说明本次函数调用发生在If的函数空间下    
        //if(DEBUG==1&&call_inst->getCalledFunction()->getName().str()=="__add_preferred_console"){
        //  errs()<<"calling basic block: "<<*(call_inst->getParent())<<"\n";
        //  errs()<<"father basic block: "<<*(PIB)->getParent()<<"\n";
        //}
        // 检验分支块中调用异常处理函数的次数
        for(unsigned i = 0;i< PIB->getNumSuccessors();i++){
          BasicBlock* tempBlock = PIB->getSuccessor(i);
          //if(DEBUG==1&&call_inst->getCalledFunction()->getName().str()=="__add_preferred_console"){
          //  errs()<<*tempBlock<<"\n";
          //}
          if(tempBlock->getName().str()[0]!='i'||tempBlock->getName().str()[1]!='f')continue;
          for (Instruction &I : *tempBlock) {
            if (auto *Call = dyn_cast<CallBase>(&I)) {
              const Function *Callee = Call->getCalledFunction();
              if(Callee && suspiciousList.count(Callee->getName())){
                partBranchErrFlag++;
                // if(DEBUG == 1) std::cout<<"Find error handler in Branch"<<std::endl;
                break;
              }
            }
          }
        }
      }
    }

    // 父块为Switch结尾时
    SwitchInst *PIB2 = dyn_cast<SwitchInst>(PI);
    if(PIB2){
      // if(DEBUG2 == 1) std::cout<<"Transform SwitchInst complete"<<std::endl;
      for(unsigned i = 0;i< PIB2->getNumCases();i++){
        BasicBlock* tempBlock = PIB2->getSuccessor(i);
        for (Instruction &I : *tempBlock) {
          if (auto *Call = dyn_cast<CallBase>(&I)) {
            const Function *Callee = Call->getCalledFunction();
            if(Callee && suspiciousList.count(Callee->getName())){
              partBranchErrFlag++;
              // if(DEBUG5 == 1) std::cout<<"Find error handler in Case, partBranchErrFlag="<<partBranchErrFlag<<" getNumCases()="<<PIB2->getNumCases()<<std::endl;
              break;
            }
          }
        }
      }
    }
  
    // 确认异常处理函数及安全检查
    if(PIB){
      if(PIB->isConditional()){
        if (partBranchErrFlag>0&&partBranchErrFlag < PIB->getNumSuccessors()){
          if(errHandlerConfirmList.count(call_inst->getCalledFunction()->getName().str())) errHandlerUsedTimes[call_inst->getCalledFunction()->getName().str()]+=1;
          else errHandlerUsedTimes[call_inst->getCalledFunction()->getName().str()] = 1;
          errHandlerConfirmList.insert(call_inst->getCalledFunction()->getName().str());
          errConfirmPath.insert(CallingPath(call_inst->getCalledFunction()->getName().str(),call_inst->getFunction()->getName().str(), call_inst->getModule()->getName().str()));
          break;
        }
      }
    }else if (PIB2){
      //if(DEBUG2 == 1) std::cout<<"i can enter here"<<std::endl;
      if (partBranchErrFlag>0&&partBranchErrFlag < PIB2->getNumCases()){
        if(errHandlerConfirmList.count(call_inst->getCalledFunction()->getName().str())) errHandlerUsedTimes[call_inst->getCalledFunction()->getName().str()]+=1;
        else errHandlerUsedTimes[call_inst->getCalledFunction()->getName().str()] = 1;

        errHandlerConfirmList.insert(call_inst->getCalledFunction()->getName().str());
        errConfirmPath.insert(CallingPath(call_inst->getCalledFunction()->getName().str(),call_inst->getFunction()->getName().str(), call_inst->getModule()->getName().str()));
        break;
      }
    }
  }
}

int detectErrFuncByName(Function& F){
   
    std::string FuncName = F.getName().str();
    
    if (detectErrByName(FuncName)) return 1;
   
    if (detectErrFuncArgByName(F)) return 1;

    return 0;
}

void basic_call_graph(Module &M){

  std::set<std::string> suspiciousList;
  std::set<std::string> errHandlerConfirmList;
  std::set<CallingPath> errConfirmPath;
  std::map<std::string, int> errHandlerUsedTimes; 
  std::set<std::string> FilteredErrConfiremed;  
  std::set<std::string> errHandleKeywords;
 
  int totalSuspiciousCall=0;
  int totalBranchOrSwitch = 0;
  
  // load the suspiciousList and kaywords
  loadStringSet(suspiciousList,"suspiciousList.txt");
  loadStringSet(errHandleKeywords,"errHandleKeywords.txt");
  loadStringSet(errHandlerConfirmList,"errHandlerConfirmList.txt");

  // input start Function
  for (Function &F : M){
    for (BasicBlock &BB : F){
      for(Instruction &I : BB){
        if (CallInst *call_inst = dyn_cast<CallInst>(&I)){
          Function *callee = call_inst->getCalledFunction();
          if(callee){
            // 都可以确定函数的前驱与后继
            // 我们去处理当前函数调用语句在一个分支语句内的情况，不去确定这个被调用函数被其他函数调用的情况。
            // 当异常处理函数被调用时，考虑其是否用于安全检查
            // 基于异常处理函数的安全检查确认，基于安全检查的异常处理函数确认
            if (suspiciousList.count(callee->getName().str())){
              //if(DEBUG3 == 1) std::cout<<F.getName().str()<<" -> "<<callee->getName().str()<<std::endl;
              // if(DEBUG4 == 1) std::cout<<"Current process error handler : No."<< totalSuspiciousCall <<" The progress: "<< totalSuspiciousCall*1.0/6242<<std::endl;
              totalSuspiciousCall++ ;
              // std::cout<<"Current process error handler : No."<< totalSuspiciousCall <<" The progress: "<< totalSuspiciousCall*1.0/6242<<std::endl;
              //if(errHandlerConfirmList.count(call_inst->getCalledFunction()->getName().str())) continue;
              // if function name include "err", this function will be recongnized as error handler function.
              if (detectErrFuncByName(*(call_inst->getCalledFunction()))) errHandlerConfirmList.insert(call_inst->getCalledFunction()->getName().str());
              // else recursive confirm this function.
              else RecursiveConfirm(call_inst,suspiciousList,errHandlerConfirmList,errConfirmPath,errHandlerUsedTimes);
            }
          }
        }
      }
    }
  }
  for(auto it:errHandlerUsedTimes){
    if(it.second>=2){
      FilteredErrConfiremed.insert(it.first);
    }
  }
   // dumpErrHandlerList(suspiciousList);
  dumpStringSet(errHandlerConfirmList,"errHandlerConfirmList.txt");
  dumpCallPath(errConfirmPath,"errConfirmPath.txt");
  // dumpUsingTimes(errHandlerUsedTimes,"errHandlerUsedTimes.txt");
  // dumpStringSet(FilteredErrConfiremed,"FilteredErrConfiremed.txt");
  // std::cout<<"total FilteredErrConfiremed: "<<FilteredErrConfiremed.size()<<std::endl;
  std::cout<<"total errHandlerConfirmList: "<<errHandlerConfirmList.size()<<std::endl;
  std::cout<<"total totalSuspiciousCall: "<<totalSuspiciousCall<<std::endl;
  std::cout<<"total totalBranchOrSwitch: "<<totalBranchOrSwitch<<std::endl;
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
