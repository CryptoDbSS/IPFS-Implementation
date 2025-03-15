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

#include "io.h"
#include "hasher.h"
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <filesystem>

#ifndef SETTINGS_H
#define SETTINGS_H

inline std::string keydata;
inline uint maxTimeForConfirmation = 650;

std::string retrievePrivFromTerminal(string msg) {

    std::string password;
    termios oldt, newt;

    // Obtener la configuración actual de la terminal
    tcgetattr(STDIN_FILENO, &oldt);

    // Desactivar la visualización de caracteres
    newt = oldt;
    newt.c_lflag &= ~ECHO;

    // Aplicar la nueva configuración
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::cout << msg;

    std::getline(std::cin, password);

    // Restaurar la configuración original de la terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return password;
}

bool savePass(){

    std::string CheckPatch = "./sets/private"; // Reemplaza con la ruta de tu directorio

    if (filesystem::exists(CheckPatch)) {
        std::cout << "There is an existing encryption key, please delete it and re-add the saved wallets again with your new pass, otherwise they will not load as they were encrypted with the existing key." <<std::endl<<"file at sets/private" <<std::endl;
        return false;
    } else {
    }

    vector <unsigned char > Data = sha3_256v(sha3_256(retrievePrivFromTerminal("Enter the new Password :")));

    saveFile("./sets/private", Data);

    return true;

}

bool SetProvider(string RPC_Address){

    std::vector<unsigned char> Data(RPC_Address.begin(), RPC_Address.end());
    saveFile("./sets/rpc", Data);
    cout<<endl<<"Privider Set"<<endl;
    return true;

}

std::string LoadProvider(){

    vector<unsigned char> dataRead;
    dataRead = readFile("./sets/rpc");

    return std::string(dataRead.begin() , dataRead.end());
    
}

bool recoveryPass(){

     std::string CheckPatch = "./sets/private"; // Reemplaza con la ruta de tu directorio

    if (!filesystem::exists(CheckPatch)) {
        std::cout << "There is no encryption key currently configured, you will need to configure it by running ./cryptodbss-client_rpc-CLI setpassencryption"<<endl<<" Also, if there are saved wallets you will need to save them again with your new password, otherwise they will not be loaded because they were encrypted with the previoud existing key.."<<std::endl;
        return false;
    } 

    vector<unsigned char> dataRead = readFile("./sets/private");
    std::string key = retrievePrivFromTerminal("Enter the Password of decrypt:");

    if( dataRead == sha3_256v(sha3_256(key))){
        keydata = bytesToStringRaw(sha3_256(key));
        key="";
        return true;
    }

    key="";
    return false;

}

inline std::string RPC_Address = LoadProvider();

#endif