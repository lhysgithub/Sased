#include "common.h"

int detectErrByName(std::string Name){

    std::string temp = "err";

    // 检测函数名是否包含小写异常字符串集元素
    if (Name.find(temp)!=std::string::npos){
      if(Name.find(temp)==0||Name[Name.find(temp)-1]=='_') return 1;
    }

    // 检测函数名是否包含大写异常字符串集元素
    transform(temp.begin(),temp.end(),temp.begin(),::toupper);
    if (Name.find(temp)!=std::string::npos){
      if(Name.find(temp)==0||Name[Name.find(temp)-1]=='_') return 1;
    }

  return 0;
}

std::vector<ARG_PAIR> getFunctionArgTypeAndNameList(Function &F){
  std::vector<ARG_PAIR> arg_pair;
  for(auto arg = F.arg_begin();arg!=F.arg_end();arg++){
    std::string tmp = arg->getName().str();
    std::string tmp2;
    int typeId = arg->getType()->getTypeID();
    switch (typeId){
      case 0:{tmp2 = "Void";break;}
      case 1:{tmp2 = "Half";break;}
      case 2:{tmp2 = "Float";break;}
      case 3:{tmp2 = "Double";break;}
      case 4:{tmp2 = "X86_FP80";break;}
      case 5:{tmp2 = "FP128";break;}
      case 6:{tmp2 = "PPC_FP128";break;}
      case 7:{tmp2 = "Label";break;}
      case 8:{tmp2 = "Metadata";break;}
      case 9:{tmp2 = "X86_MMX";break;}
      case 10:{tmp2 = "Token";break;}
      case 11:{tmp2 = "Integer";break;}
      case 12:{tmp2 = "Function";break;}
      case 13:{tmp2 = "Struct";break;}
      case 14:{tmp2 = "Array";break;}
      case 15:{tmp2 = "Pointer";break;}
      case 16:{tmp2 = "Vector";break;}
    }
    arg_pair.push_back(make_pair(tmp2,tmp));
    // errs()<<"Type: "<<tmp2<<" Value: "<<tmp<<"\n";
  }
  return arg_pair;
  // exit(100);
}

std::vector<ARG_PAIR> getCallArgTypeAndNameList(CallInst *CInst){
  std::vector<ARG_PAIR> arg_pair;
  for(unsigned i=0;i<CInst->getNumArgOperands();i++){
    Value *op0 = CInst->getArgOperand(i);
    std::string tmp = op0->getName().str();
    int typeId = op0->getType()->getTypeID();
    std::string tmp2;
    switch (typeId){
      case 0:{tmp2 = "Void";break;}
      case 1:{tmp2 = "Half";break;}
      case 2:{tmp2 = "Float";break;}
      case 3:{tmp2 = "Double";break;}
      case 4:{tmp2 = "X86_FP80";break;}
      case 5:{tmp2 = "FP128";break;}
      case 6:{tmp2 = "PPC_FP128";break;}
      case 7:{tmp2 = "Label";break;}
      case 8:{tmp2 = "Metadata";break;}
      case 9:{tmp2 = "X86_MMX";break;}
      case 10:{tmp2 = "Token";break;}
      case 11:{tmp2 = "Integer";break;}
      case 12:{tmp2 = "Function";break;}
      case 13:{tmp2 = "Struct";break;}
      case 14:{tmp2 = "Array";break;}
      case 15:{tmp2 = "Pointer";break;}
      case 16:{tmp2 = "Vector";break;}
    }
    //std::cout<<"Ctmp: "<<tmp<<" Ctmp2: "<<tmp2<<"\n";
    arg_pair.push_back(make_pair(tmp2,tmp)); // 类型和对象
  }
  return arg_pair;
}


int detectSusNameByKeyWord(std::string FuncName,std::set<std::string> errHandleKeywords){

  for (std::string errandleKeyword : errHandleKeywords){
    std::string temp = errandleKeyword;

    // 检测函数名是否包含小写异常字符串集元素
    if (FuncName.find(temp)!=std::string::npos){
      if(temp == "err"){
        if(detectErrByName(FuncName)) return 2;
      }
      else return 1;
    }

    // 检测函数名是否包含大写异常字符串集元素
    transform(temp.begin(),temp.end(),temp.begin(),::toupper);
    if (FuncName.find(temp)!=std::string::npos){
      if(temp == "ERR"){
        if(detectErrByName(FuncName)) return 2;
      }
      else return 1;
    }
  }
  return 0;
}

void loadStringSet(std::set<std::string>& stringSet,std::string loadFile){
  std::string temp;
  std::ifstream os;
  os.open(loadFile);
  if(os.is_open()){
        while(os>>temp){
          stringSet.insert(temp);
        }
  }
}

int dumpStringSet(std::set<std::string>& dumpStringList,std::string outputFile){
  std::string dumpString="";
  for(auto it : dumpStringList){
      dumpString += it+"\n";
  }

  std::ofstream os;
  os.open(outputFile);
  if (os.is_open()) {
    os<<dumpString<<"\n";
    os.close();
  }
  return 1;
}

int detectErrFuncArgByName(Function& F){
  std::string temp;
  std::vector<ARG_PAIR> farg_pair = getFunctionArgTypeAndNameList(F);
  for(auto funcarg : farg_pair){
    // if the arg of function include err
    temp = "err";
    if(funcarg.second.find(temp)!=std::string::npos){
      if(funcarg.second.find(temp)==0||funcarg.second[funcarg.second.find(temp)-1]=='_') return 1;
    }
    // if the arg of function include ERR
    temp = "ERR";
    if(funcarg.second.find(temp)!=std::string::npos){
      if(funcarg.second.find(temp)==0||funcarg.second[funcarg.second.find(temp)-1]=='_') return 1;
    }
  }
  return 0;
}
