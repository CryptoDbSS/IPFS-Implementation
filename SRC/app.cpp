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

#include "callfunc.h"

bool SignalThread;

int main(int argc, char* argv[]){ 

    if (argc < 2) {
        cout<<endl<<"few arguments at call "<<endl;
        return 1;
    }

    std::string argCalled = argv[1];

    if(argCalled=="PostMSG"){
        _PostMSG(argv, argc);
        thread ipfs_daemon(Run_Daemon);
        ipfs_daemon.join();
    }

    if(argCalled=="PostFile"){
        _PostFile(argv, argc);
        thread ipfs_daemon(Run_Daemon);
        ipfs_daemon.join();

    }

    if(argCalled=="ReadMSG"){ 
        _RetrieveMsg(argv, argc);
    }

    if(argCalled=="daemon"){
        Run_Daemon();
        cout<<endl<<"daemon debyg "<<endl;
    }

    //Set rpc address e.g https://0.0.0.0:18090
    if(argCalled=="setprovider"){
        _SetProvider(argv, argc);
        return 0;
    }



    return 0;

}