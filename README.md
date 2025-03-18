<b>CryptoDbss</b> allows short messages to be published on the blockchain as a transaction. This can be annotated to be linked as metadata, for example, the SHA of a data or document. In this case, an example of its use with IPFS will be given. <br><br>

What is IPFS? <br><br>

Interplanetary File System, as its acronym <b>IPFS</b>, is a decentralized network protocol that allows its users to share any file, or data between connected nodes. It can also be called <b>Web3</b> protocol, therefore anyone running a node can share or download what others share, without the need for a centralized server. These files, in turn, can be in directories, which allows for more orderly and precise indexing, to access them, it is necessary to know the <b>CID</b> (content identifier). This is generated when the file is uploaded. Each <b>CID</b> for each file and directory is unique, so there is only one unique one for each one, just like a hash, which is generally represented in base58. <br><br>

Many <b>decentralized apps</b> and <b>DAO's</b> that use the term <b>web3</b> implement it through <b>IPFS</b>.<br><br>

Some advantages of implement <b>IPFS</b> <br/>
 <ol>
  <li>Save space on long messages, resulting in lighter and lower-cost transactions.</li>
  <li>complete decentralization of the data that is shared.</li>
  <li>share associated metadata or files in blockchain without them taking up space within it.</li>
  <li>Increase capabilities to make more Web3 implementations.</li>
</ol> 

Okay, and now how do we implement the <b>IPFS</b> protocol in <b>cryptoDBSS</b>??<br><br>

As mentioned above, <b>CryptoDBSS</b> allows hosting short messages, 32 bytes in size to be exact. The <b>CID</b> can be coded into base16 and be reinterpreted for use as a post message on <b>CryptoDbSS</b>. This can then be read from the blockchain and be  reconstructed as a <b>CID</b> using <b>base32</b> or <b>base58</b>, and then, will recover the data from <b>IPFS</b>.<br><br>

On the cryptoDBSS GitHub, there is already a repo illustrating the use of this implementation, using a local client and kubo (the official IPFS version in Go).<br><br>

The command is executed to post a file containing the following message in a transaction:<br>

<p align="center"> <blockquote>"hello, 你好 , this is a message of CryptoDbSS using IPFS."</blockquote> </p><br>

<div class="separator" style="clear: both;"><a href="https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEg6f4wgPsdsEYCV_8fbGXEgQBFJdm_JeCtd3e6kwvWQSXsY24kO4pUHnulyAAJXDP5tEedeqXQy_KVOkrCmYPE44pEvrP1Jp3OlQb0t1WCX7dAGrhqJONky2r0O3gge_8yBg1h-CK3LZIePKrTDoBMMIwy1OxYJt2gVsLxITYs0D-LUIaHH_aPzr4uJ9yA/s1600/img0.jpg" style="display: block; padding: 1em 0; text-align: center; "><img alt="" border="0" data-original-height="1080" data-original-width="1920" src="https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEg6f4wgPsdsEYCV_8fbGXEgQBFJdm_JeCtd3e6kwvWQSXsY24kO4pUHnulyAAJXDP5tEedeqXQy_KVOkrCmYPE44pEvrP1Jp3OlQb0t1WCX7dAGrhqJONky2r0O3gge_8yBg1h-CK3LZIePKrTDoBMMIwy1OxYJt2gVsLxITYs0D-LUIaHH_aPzr4uJ9yA/s1600/img0.jpg"/></a></div>


The terminal indicates that everything proceeded correctly, both the network file sharing on IPFS and the transaction containing the CID in CryptoDbSS. <br><br>

Then we retrieve the CID and use IPFS to access the data. <br>

<div class="separator" style="clear: both;"><a href="https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEjJJOV_3N8tT-0YnOtQ9gnwCTsmb5bE46OhAxBaR29Rt-7gMwmInZZ81Cjr3McWyWbl6M-nya18P3UKpWQbMqVN9rvhw1dU2S6_h_DSDTxf8lILaXYDOGUr8F6ps3Yy7IJa7oNJjmOJdB-wu8vJvtq8dMhPV7JCzaL4Wp3FwVAuKEhaVHZexVjGwoS3UMc/s1600/img0.jpg" style="display: block; padding: 1em 0; text-align: center; "><img alt="" border="0" data-original-height="1080" data-original-width="1920" src="https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEjJJOV_3N8tT-0YnOtQ9gnwCTsmb5bE46OhAxBaR29Rt-7gMwmInZZ81Cjr3McWyWbl6M-nya18P3UKpWQbMqVN9rvhw1dU2S6_h_DSDTxf8lILaXYDOGUr8F6ps3Yy7IJa7oNJjmOJdB-wu8vJvtq8dMhPV7JCzaL4Wp3FwVAuKEhaVHZexVjGwoS3UMc/s1600/img0.jpg"/></a></div>
