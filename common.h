#ifndef COMMON_H
#define COMMON_H

#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <fstream>

using namespace llvm; // I know this is bad.

// ARG_PAIR (type,name)
typedef std::pair<std::string, std::string> ARG_PAIR;

// detect err from string
int detectErrByName(std::string Name);

// detect err from args of function
int detectErrFuncArgByName(Function& F);

// detect sus from string
int detectSusNameByKeyWord(std::string FuncName,std::set<std::string> errHandleKeywords);

// get function args
std::vector<ARG_PAIR> getFunctionArgTypeAndNameList(Function &F);

// get calling args
std::vector<ARG_PAIR> getCallArgTypeAndNameList(CallInst *CIns);

// load string set
void loadStringSet(std::set<std::string>& stringSet,std::string loadFile);

// dump string set
int dumpStringSet(std::set<std::string>& dumpStringList,std::string outputFile);

// include implementation
#include "common.cpp"

#endif /* COMMON_H */

