#include "../common.h"
#include <llvm/Analysis/CallGraph.h>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#define TRUE 1
#define FALSE 0
#define DEBUG 1

int isSimilar(Function &F,CallInst *call_inst, std::set<std::string> errHandleKeywords){
  std::vector<ARG_PAIR> carg_pair = getCallArgTypeAndNameList(call_inst);
  std::vector<ARG_PAIR> farg_pair = getFunctionArgTypeAndNameList(F);
  // int count=0;
  for(auto callarg : carg_pair){
    for(auto funcarg : farg_pair){
      // if(callarg.second!="" && callarg.second == funcarg.second) return 1;
      // if caller and callee has the same argrement, extend it.
      int ans = detectSusNameByKeyWord(callarg.second,errHandleKeywords);
      if(ans && callarg.second == funcarg.second){
         if (ans==2) return 2;
         return 1;
      }
    }
  }
  return 0;
}


void basic_call_graph(Module &M){
  int caller_callee_both_errHandler_num = 0;
  std::set<std::string> suspiciousList;
  std::set<std::string> errHandlerConfirmList;
  std::set<std::string> errHandleKeywords;

  // load the suspiciousList and errHandlerConfirmList
  loadStringSet(errHandlerConfirmList,"errHandlerConfirmList.txt");
  loadStringSet(suspiciousList,"suspiciousList.txt");
  loadStringSet(errHandleKeywords,"errHandleKeywords.txt");

  for(Function& F : M){
    for(BasicBlock& BB : F){
      for(Instruction& I : BB){
        if (CallInst *call_inst = dyn_cast<CallInst>(&I)){
          Function *callee = call_inst->getCalledFunction();
          if(callee){
            // if callee and caller both are confirmed error handler function, count +1 and contine 
            if(errHandlerConfirmList.count(callee->getName().str()) && errHandlerConfirmList.count(F.getName().str())) { 
              caller_callee_both_errHandler_num++;
              continue;
            }

            // if callee is confirmed error handler, add the caller in suspicious
            if (errHandlerConfirmList.count(callee->getName().str())){
              if(isSimilar(F,call_inst,errHandleKeywords)==1) suspiciousList.insert(F.getName().str());
              else if (isSimilar(F,call_inst,errHandleKeywords)==2) errHandlerConfirmList.insert(F.getName().str());
            }

            // if callee is confirmed error handler, add the caller in suspicious
            else if(errHandlerConfirmList.count(F.getName().str())){
              if(isSimilar(F,call_inst,errHandleKeywords)==1) suspiciousList.insert(callee->getName().str());
              else if (isSimilar(F,call_inst,errHandleKeywords)==2) errHandlerConfirmList.insert(callee->getName().str());
            }
          }
        }
      }
    }
  }
  dumpStringSet(errHandlerConfirmList,"errHandlerConfirmList.txt");
  std::cout<<"total errHandlerConfirmList: "<<errHandlerConfirmList.size()<<std::endl;
  dumpStringSet(suspiciousList,"suspiciousList.txt");
  std::cout<<"total suspiciousList: "<<suspiciousList.size()<<std::endl;
}

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

