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

#include "IPFS_kubo_wrapper.h"
#include "CryptoDbSS-ClientRPC.h"

string prefix = "f01701220";


string PostFile(const string &patch, string &key, const string &value){

    string CID = addFile(patch);
    string CIDFormated = CIDToBase16(CID);
    string CIDFormatedcut = CIDFormated.substr(9,64);
    ProcessTransaction2(key, CIDFormatedcut, value, 0x0A);

    return CIDToBase58(CIDFormated);

}

string PostMSG(const string &msg, string &key, const string &value){

    string CID = addStr(msg);
    cout<<endl<<"CID "<<CID<<endl;
    string CIDFormated = CIDToBase16(CID);
    cout<<endl<<"CID formated base16  "<<CIDFormated<<endl;
    string CIDFormatedcut = CIDFormated.substr(9,64);
    cout<<endl<<"CID data base16 prefix "<<CIDFormatedcut<<endl;

    ProcessTransaction2(key, CIDFormatedcut, value, 0x0A);

    return CIDToBase58(cidtorecoverer);

}

string RetrieveMsg(const string &BN, const string &TN){

    string result = SearchTransactionMSGByNumer(BN, TN);

    if(result.length()==64){

        string prefix = "f01701220";
        string req_ipfs = prefix+result;
        req_ipfs= CIDToBase58(req_ipfs);
        cout<<endl<<"CryptoDbSS-IPFS CIDv1 base58 from: "<<req_ipfs;
        result = cat(req_ipfs);
    } else{ 
        result = "could not retrieve message, response: "+result;
    }

    return result;

}