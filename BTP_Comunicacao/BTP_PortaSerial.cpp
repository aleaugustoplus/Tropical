/*##################################################################
  #                Objeto de Comunica��o Serial                    #
  #               Copyright�:                                      #
  #                Tropical� Corp.                                 #
  #----------------------------------------------------------------#
  #               Modificada em:                                   #
  #               29/05/2007  - Alexandre A. S. Lopes              #
  #----------------------------------------------------------------#
  #               30/05/2007  - Alexandre A. S. Lopes              #
  #----------------------------------------------------------------#
  #               11/07/2007  - Alexandre A. S. Lopes              #
  #----------------------------------------------------------------#
  #               26/10/2007  - Alexandre A. S. Lopes              #
  #----------------------------------------------------------------#
  #               09/11/2007  - Alexandre A. S. Lopes              #
  #----------------------------------------------------------------#
  #               19/11/2007  - Alexandre A. S. Lopes              #
  ##################################################################*/

//---------------------------------------------------------------------------
#include "BTP_PortaSerial.h"
#include <string.h>
#include <stdio.h>
//---------------------------------------------------------------------------
__TROPICALL BTP_PortaSerial::BTP_PortaSerial(char * nome,BTP_Porta tPorta):BTP_PortaCom(nome)
{
 char *cPorta;
 switch(tPorta)
 {
  case COM1:
   cPorta="COM1";
  break;
  case COM2:
   cPorta="COM2";
  break;
  case COM3:
   cPorta="COM3";
  break;
  case COM4:
   cPorta="COM4";
  break;
 }

 hporta = CreateFile(cPorta,
			 GENERIC_READ | GENERIC_WRITE,
			 0, // dispositivos comm abertos com acesso exclusivo
			 NULL, // sem atributos de seguran�a
			 OPEN_EXISTING, // deve usar OPEN_EXISTING
			 FILE_FLAG_OVERLAPPED,      //Entrada e sa�da sem overlap.
			 NULL   // hTemplate deve ser NULL para comm
		  );

  if(hporta==INVALID_HANDLE_VALUE)
   throw new BTP_Excecao(this," N�o foi poss�vel abrir a porta de comunica��o especificada! \n \
Ela pode estar sendo usada por outro programa");

  //this->ConfiguraControle();
  //this->ConfiguraTimeOuts();



   tamanhobuffer=0;

   bufferport = new BTP_BufferPorta("Buffer Porta",hporta);

   this->ConfiguraControle();
   this->ConfiguraTimeOuts();
//   bufferport->Porta=hporta;



}
//---------------------------------------------------------------------------
__TROPICALL BTP_PortaSerial::~BTP_PortaSerial()
{
// BTP_Thread::TerminaThread();


 CloseHandle(hporta);
}
//---------------------------------------------------------------------------
void __TROPICALL BTP_PortaSerial::Objeto()
{

}
//---------------------------------------------------------------------------
void __TROPICALL BTP_PortaSerial::TransmitirByte(BTP_PortaCom::Dado Buffer)
{
 DWORD nBytesRead;

 if(!WriteFile(hporta, &Buffer, 1, &nBytesRead, NULL))
  throw new BTP_Excecao(this,"N�o foi possivel escrever na porta");


}
//---------------------------------------------------------------------------
void __TROPICALL BTP_PortaSerial::TransmitirDados(BTP_PortaCom::Dado *Buffer,unsigned int Tamanho)
{
  DWORD nBytesRead;
  WriteFile(hporta, Buffer, Tamanho, &nBytesRead, &bufferport->ovsaida);
//  throw new BTP_Excecao(this,"N�o foi possivel escrever na porta");
}
//---------------------------------------------------------------------------
void __TROPICALL BTP_PortaSerial::LeituraAsinc()
{
	int     numRead;
		// Solicita leitura com overlap de um caracter
		 ReadFile(
					(HANDLE)hporta,  // socket
					bufferport->entrada,
					(DWORD)1,
					(LPDWORD)&numRead,
					 &bufferport->ovIn // Ponteiro para estrutura overlap	+ ap�ndices
				);



}
//---------------------------------------------------------------------------
BTP_PortaCom::Dado __TROPICALL BTP_PortaSerial::ReceberByte()
{
 DWORD nBytesRead;
 unsigned char temp;

 if(!ReadFile(hporta, &temp, 1, &nBytesRead, NULL))
  throw new BTP_Excecao(this,"N�o foi possivel ler na porta");


 return temp;
}
//---------------------------------------------------------------------------
bool __TROPICALL BTP_PortaSerial::ConfiguraControle()
{
DCB dcb;
   if(!GetCommState(hporta, &dcb))
	 return false;

   dcb.BaudRate = CBR_9600;
   dcb.ByteSize = 8;
   dcb.Parity = NOPARITY;
   dcb.StopBits = ONESTOPBIT;

   if( SetCommState(hporta, &dcb) == 0 )
	  return false;

   return true;
}
//---------------------------------------------------------------------------
bool __TROPICALL BTP_PortaSerial::ConfiguraTimeOuts()
{
COMMTIMEOUTS CommTimeouts;

   if( GetCommTimeouts(hporta, &CommTimeouts) == 0 )
	  return false;


   CommTimeouts.ReadIntervalTimeout = 500;
   CommTimeouts.ReadTotalTimeoutMultiplier = 10;
   CommTimeouts.ReadTotalTimeoutConstant = 5000;
   CommTimeouts.WriteTotalTimeoutMultiplier = 5;
   CommTimeouts.WriteTotalTimeoutConstant = 5;

   if( SetCommTimeouts(hporta, &CommTimeouts) == 0 )
	  return false;

   return true;
}
//--Propriedades-------------------------------------------------------------
HANDLE __TROPICALL BTP_PortaSerial::PegarhPorta()
{
 return hporta;
}
//---------------------------------------------------------------------------
BTP_PortaSerial::BTP_PortaConfig __TROPICALL BTP_PortaSerial::PegarPortaConfig()
{
 DCB dcb;
 if(!GetCommState(hporta, &dcb))
   throw new BTP_Excecao(this,"N�o foi possivel obter configura��es da porta!");

return dcb;
}
//---------------------------------------------------------------------------
void __TROPICALL BTP_PortaSerial::AlterarPortaConfig(BTP_PortaConfig tconfig)
{
 if(!SetCommState(hporta, &tconfig))
  throw new BTP_Excecao(this,"N�o foi possivel configurar a porta!");
}
//---------------------------------------------------------------------------
HANDLE __TROPICALL BTP_PortaSerial::PegarPorta()
{
 return hporta;
}
//---------------------------------------------------------------------------
BTP_PortaSerial::MascaraCom __TROPICALL BTP_PortaSerial::PegarMascara()
{
 DWORD Mask;
 if(!GetCommMask(hporta,&Mask))
	throw new BTP_Excecao(this,"N�o foi possivel obter configurara��o de m�scara de comunica��o!");
 return (MascaraCom)Mask;
}
//---------------------------------------------------------------------------
void __TROPICALL BTP_PortaSerial::AlterarMascara(MascaraCom Mask)
{
 if(!SetCommMask(hporta,Mask))
   	throw new BTP_Excecao(this,"N�o foi possivel configurar m�scara de comunica��o!");
}
//---------------------------------------------------------------------------
BTP_BufferPorta *__TROPICALL BTP_PortaSerial::PegarBufferPort()
{
	return bufferport;
}
