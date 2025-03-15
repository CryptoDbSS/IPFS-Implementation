/*******************************************************************************

 * This notice, including the copyright notice and permission notice, 
 * must be retained in all copies or substantial portions of the Software and 
 * in all derivative works.
 *
 * Software Name: CryptoDbSS-IPFS-Implementation
 * Copyright (C) 2025 Steeven J Salazar.
 * License: CryptoDbSS: Software Review and Audit License
 * 
 * https://github.com/CryptoDbSS/IPFS-Implementation
 *
 * IMPORTANT: Before using, compiling, or doing anything with this software,
 * you must read and accept the terms of the License provided with this software.
 *
 * If you do not have a copy of the License, you can obtain it at the following link:
 * https://github.com/CryptoDbSS/IPFS-Implementation/blob/main/LICENSE.md
 *
 * By using, compiling, or modifying this software, you implicitly accept
 * the terms of the License. If you do not agree with the terms,
 * do not use, compile, or modify this software.
 * 
 * This software is provided "as is," without warranty of any kind.
 * For more details, see the LICENSE file.

********************************************************************************/

#include "funcAlg.h"

void ShowHelpInfo(const string &processAppName ,const string &help){

    if(help == "ProcessTransaction"){
        cout<<processAppName+ " ProcessTransaction <fromAcc> <AddressTo or message> <value transaction> <type of transaction>"<<endl;
        return;
    }

    if(help == "setprovider"){
        cout<<processAppName+" setprovider <rpc address>"<<endl;
        return;
    }

    if(help == "IndexBalance"){
        cout<<processAppName+" IndexBalance <address Account to index>"<<endl;
        return;
    }

    if(help == "SearchTransaction"){
        cout<<processAppName+" SearchTransaction <option: bynumber, hash, MSG>  <block bumber of transaction> <number of transction>"<<endl<<
        processAppName+" SearchTransaction <option: byhash>  <hash of transaction>"<<endl;
        return;
    }

    if(help == "SearchBlock"){
        cout<<" ./cryptosbssClient  SearchBlock <number of block to index>"<<endl;
        return;
    }

    std::cout<<  std::endl;
    std::cout<< "CryptoDbSS Client commands  <funtion>            <args...>" << std::endl;
    std::cout<<  std::endl;
    std::cout<< "general:               <ProcessTransaction>      <args...>" << std::endl;
    std::cout<< "                       <IndexBalance>            <args...>" << std::endl;
    std::cout<< "                       <SearchTransaction>       <args...>" << std::endl;
    std::cout<< "                       <SearchBlock>             <args...>" << std::endl;
    std::cout<< "                       <walletsStored>             " << std::endl;
    std::cout<<  std::endl;
    std::cout<< "Accounts management:   <SaveNewWallet>           <args...>" << std::endl;
    std::cout<<  std::endl;
    std::cout<< "Settings:              <setpassencryption>       <args...>" << std::endl;
    std::cout<< "                       <setprovider>             <args...>" << std::endl;
    std::cout<<  std::endl;
    std::cout<< "      ||  Developed by:   steevenjavier@gmail.com  ||"  << std::endl;
    std::cout<<  std::endl;

}

string _PostMSG(char* arg[], uint NumbArgs){

    string result ;
    string prefix = "f01701220";

    if (NumbArgs < 5) {
        result = "few arguments at call " ;
        return result;
    }

    string msg = arg[2];
    string key = arg[3];
    string value = arg[4];

    result = PostMSG(msg, key, value);

    cout<<endl<<"Resource CID : "<<result<<endl;

    return result;

}

bool _PostFile(char* arg[], uint NumbArgs){ 



    if (NumbArgs < 5) {
        cout<<endl<<"few arguments at call " ;
        return false;
    }

    const string patch = arg[2];
    const string value = arg[4];
    string key = arg[3];

    string result = PostFile(patch, key, value);

    cout<<endl<<"Resource CID : "<<result<<endl;
    
    return true;

}

bool _RetrieveMsg(char* arg[], uint NumbArgs){

    if (NumbArgs != 4) {
        cout<<endl<<"few arguments at call " ;
        return false;
    }

    if( !HexCheck(arg[2]) || !HexCheck(arg[3]) ){
        cout<<endl<<"invalid format arguments at call " ;
        return false;
    }

    const string BN = ullToHex(stoull(arg[2])); 
    const string TN = ullToHex(stoull(arg[3]));
    string result = RetrieveMsg(BN, TN);

    cout<<endl<<"IPFS DATA: "<< result<<endl;

    return true;

}

bool _SetProvider(char* arg[], uint NumbArgs){

    const string processAppName = arg[0];
    if (NumbArgs != 3) {
        const string help ="setprovider";
        ShowHelpInfo(processAppName,help);
        return false;
    }

    std::string address = arg[2];

    return SetProvider(address);

}

