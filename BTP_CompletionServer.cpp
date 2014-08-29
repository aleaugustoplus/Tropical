//---------------------------------------------------------------------------



#include "BTP_CompletionServer.h"
#include <stdlib.h>
#include <stdio.h>
//---------------------------------------------------------------------------
BTP_CompletionServer::BTP_CompletionServer(char*nome):BTP_Thread(nome)
{
 PLC = new PLC_Controle("Controlador");
 Atualizador = new BTP_ServerAtualiza("Atualizador",PLC);
 Sock=new BTP_SocketServer("Servidor");
 CompletionPort = new BTP_CompletionPort("Completion Port");

// BTP_Thread::Ativa=true;
}
//---------------------------------------------------------------------------
BTP_CompletionServer::~BTP_CompletionServer()
{
 delete PLC;
 for(int x=0;x<4;x++)
  delete Threads[x];

 delete Sock;
 delete CompletionPort;

}
//---------------------------------------------------------------------------
void __TROPICALL BTP_CompletionServer::Executar()
{


 for(int x=0;x<4;x++)
 {
  Threads[x] = new BTP_WorkerThread("Threads",CompletionPort,PLC,Atualizador);
  Threads[x]->ThreadMsg = Atualizador->dwThreadID;
 }



 CompletionPort->AssociarArquivo(Sock->SockP);

 for(;;)
 {
  BTP_Socket* Temp = Sock->AguardarConexao();
  Atualizador->AddCliente(Temp);
  CompletionPort->AssociarArquivo(Temp);
  printf("\nCliente conectado!\n");
 }


}
//---------------------------------------------------------------------------
void BTP_CompletionServer::Iniciar()
{
 BTP_Thread::Ativa=true;
 Atualizador->Ativa = true;



}
//---------------------------------------------------------------------------
