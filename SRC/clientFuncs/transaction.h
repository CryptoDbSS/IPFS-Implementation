/*******************************************************************************
 *
 * Software Name: CryptoDbSS Client API RPC
 * Copyright (C) 2025 Steeven J Salazar.
 * License: CryptoDbSS: Software Review and Audit License
 * 
 * https://github.com/CryptoDbSS/CryptoDbSS-Client-API-RPC
 *
 * This notice, including the copyright notice and permission notice, 
 * must be retained in all copies or substantial portions of the Software and 
 * in all derivative works.
 *
 * IMPORTANT: Before using, compiling, or doing anything with this software,
 * you must read and accept the terms of the License provided with this software.
 *
 * If you do not have a copy of the License, you can obtain it at the following link:
 * https://github.com/CryptoDbSS/CryptoDbSS-Client-API-RPC/blob/main/LICENSE.md
 *
 * By using, compiling, or modifying this software, you implicitly accept
 * the terms of the License. If you do not agree with the terms,
 * do not use, compile, or modify this software.
 * 
 * This software is provided "as is," without warranty of any kind.
 * For more details, see the LICENSE file.
 *
 ********************************************************************************/


/*********************************************************************************
 
    The CryptoDbSS, blockchain-core, consensus, protocols and misc.

    This software is a review and audit release, it should only be used for 
    development, testing, education and auditing purposes. 

    Third-party dependencies: Crypto++, OpenSSL, libcurl.

    questions, suggestions or contact : Steevenjavier@gmail.com

                                S.S

*********************************************************************************/

#include "TransactionType.h"
#include "curlpostAsync.h"
#include "iostream"
#include "hasher.h"
#include <chrono>
#include <thread>

#ifndef TRANSACTION_H
#define TRANSACTION_H

bool success = false;

inline std::string recovered;
std::string TransactionData[3];
std::string publicAddress ;
uint timeConfirmation;
uint8_t TypeTransaction;
inline std::string toDir; 
inline std::string Value;

extern std::string RPC_Address;

uint8_t transacConfirmedCheck(){

    string hashblindex = curlpost(RPC_Address+"/shaLBB", "", 2000);

    if(hashblindex.length() == 64 && HexCheck(hashblindex)){

        if(TransactionData[0] != hashblindex){

            string typeindex = "2";
            string jsonval = "{\"typeIndex\": \"" + typeindex + "\", \"valueC\": \"" + TransactionData[2] + "\"}"; 
            string response = curlpost(RPC_Address+"/DataTransacIndex", jsonval, 2000);
            cout<<endl<<"transacComfirmedCheck response "<<response<<" "<<SHAstg(response)<<" "<<TransactionData[2]<<endl;

            if (response.length()==TransactionDataFormat[TypeTransaction].size_fullTransaction_String && SHAstg(response) == TransactionData[2]){

                return 0;
                
            } else if (response == "Not Found in DB"){
                return 1;
       
            }
        }
        return 1;
    }
    return 2;
}

//check if the data transaction provided by RPC is the requested by client before sign transaction. Mitigates phishing attacks.
bool CheckDataTransaction(string Data){

    if(!( (Data.length() == TransactionDataFormat[TypeTransaction].POS_signatureWithHidden_string && (TypeTransaction == 0x00 || TypeTransaction == 0x0A ) ) 
    || (Data.length() == TransactionDataFormat[0x0C].POS_signatureWithHidden_string && TypeTransaction == 0x0A  ) )){
        return false;
    }

    if( TransactionDataFormat[TypeTransaction].haveAccR && Data.substr(TransactionDataFormat[TypeTransaction].POS_addressR_string, TransactionDataFormat[TypeTransaction].size_address_string)==toDir && 
        Data.substr(TransactionDataFormat[TypeTransaction].POS_transactionValue_string, TransactionDataFormat[TypeTransaction].size_value_string)==Value){
        return true;
    }

    if( !TransactionDataFormat[TypeTransaction].haveAccR && Data.substr(TransactionDataFormat[TypeTransaction].POS_Msg32bytes_string, TransactionDataFormat[TypeTransaction].size_Msg32bytes_string)==toDir && 
        Data.substr(TransactionDataFormat[TypeTransaction].POS_transactionValue_string, TransactionDataFormat[TypeTransaction].size_value_string)==Value){
        return true;
    }

    return true;

}

uint8_t CheckAndFirm(){

    string hashblindex = curlpost(RPC_Address+"/shaLBB", "", 2000);

    if(hashblindex.length() == 64 && HexCheck(hashblindex)){
        if(TransactionData[0] == hashblindex){

            string response = curlpost(RPC_Address+"/queuetransacs", TransactionData[1], 2000);

            if(CheckDataTransaction(response)){

                string sign = Signer(recovered, response);
                TransactionData[1] = response + sign;

                TransactionData[2] = SHAstg( TransactionData[1]);

                string jsonval = "{\"x\": \"" + TransactionData[1] + "\"}";

                //transaccion enviada al nodo
                response = curlpost(RPC_Address+ "/TransacSignedPost", jsonval, 2000);

                cout << endl<< "RPC response status: "<< response<<endl;

                if(response == "SUCCESS"){ 
                    return 0;
                }
            } else {
                if(response != "PROCESSING"){
                    return 1;
                }
            }
        }
    }
    cout<<endl;
    return 2;

}

bool getDataTransac(string toDir, string value){

    value = ullToHex(hexToULL(value));

    std::string typet  =  byteToHex2(TypeTransaction);

    string shaLBB = TransactionData[0];
    string stg1= shaLBB+typet+publicAddress+toDir+value;

    string signature = Signer(recovered, stg1);


    string jsonval = "{\"v\": \"" + typet + "\", \"w\": \"" + publicAddress + "\", \"x\": \"" + toDir + "\", \"y\": \"" + value + "\", \"z\": \"" + signature + "\"}"; 

    std::string response = curlpost(RPC_Address+ "/GetDataTransac", jsonval, 2000);

    if(response.length()== 64){
        cout<<endl<<"transaction req ID :"<<response;
        TransactionData[1] = response;
        return true;
    } else {
        cout<<endl<<"Failed to request Data Transaction, status:"<<response<<endl;
        return false; 
    }

}

void loopPendingCheck(){

    uint8_t result = 1;

    while(result = 1){

        if(timeConfirmation < timing()){
            return;
        }       

        result = CheckAndFirm();

        switch(result){
            case 0x00:
            case 0x01:
                break;
            case 0x02: 
                return;
            default:
                return;
        }

        std::this_thread::sleep_for(std::chrono::seconds(4));

    }

    result = 1;

    while(result = 1){

        if(timeConfirmation < timing()){
            break;
        }  

        transacConfirmedCheck();

    }

}

bool getDataTransac(uint8_t &type){

    string shaLBB = curlpost(RPC_Address+ "/shaLBB", "", 2000);

    if(shaLBB.length()== 64){

        TransactionData[0]= shaLBB;
        TypeTransaction = type;

        switch(TypeTransaction){
            case 0x00:
            case 0x0A:
                return getDataTransac(toDir, Value);
                break;
        }

    } 
    return false ; 
    
}

bool ProcessTransaction(std::string &fromAcc, std::string &todir, std::string value, uint8_t TypeTran){

    instanceElementsTransaction();

    if(loadWallet(fromAcc)){
        
        recovered = bytesToStringRaw(AccLoaded);
        AccLoaded.clear();
        publicAddress = derivate(recovered);

        for (auto &s : todir){s = toupper(s);}

        toDir = todir;
        Value = value;

        for(uint8_t tries=3 ; tries>0; tries--){
            if (getDataTransac(TypeTran)){
                timeConfirmation = timing()+maxTimeForConfirmation;
                loopPendingCheck();
                break;
            }
            if(tries == 1){
                cout<<"The transaction data request could not be processed"<<endl;
                return false;
            }
        }
        recovered="";
        AccLoaded.clear();
        return true;
    }   else{
        cout<<"The Account couln't be loaded"<<endl;
    }
    return false;
}

bool ProcessTransaction2(std::string &fromAcc, std::string &todir, std::string value, uint8_t TypeTran){

    instanceElementsTransaction();

    recovered= fromAcc;
    publicAddress = derivate(fromAcc);

    for (auto &s : todir){s = toupper(s);}

    toDir = todir;
    Value = value;

    for(uint8_t tries=3 ; tries>0; tries--){
        if (getDataTransac(TypeTran)){
            timeConfirmation = timing()+maxTimeForConfirmation;
            loopPendingCheck();
            break;
        }
        if(tries == 1){
            cout<<"The transaction data request could not be processed"<<endl;
        }
    }
    fromAcc="";
    AccLoaded.clear();
    return 0;

}

bool ProcessTransactionDebugNoSign(std::string &fromAcc, std::string &todir, std::string value, uint8_t TypeTran){

    instanceElementsTransaction();

    if(loadWallet(fromAcc)){
        
        recovered = bytesToStringRaw(AccLoaded);
        AccLoaded.clear();
        publicAddress = derivate(recovered);

        for (auto &s : todir){s = toupper(s);}

        toDir = todir;
        Value = value;

        for(uint8_t tries=3 ; tries>0; tries--){
            if (getDataTransac(TypeTran)){
                break;
            }
            if(tries == 1){
                cout<<"The transaction data request could not be processed"<<endl;
                return false;
            }
        }
        recovered="";
        AccLoaded.clear();
        return true;
    }   else{
        cout<<"The Account couln't be loaded"<<endl;
    }
    return false;
}

#endif

