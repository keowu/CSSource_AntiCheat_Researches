/****************************************************************************
 *                                                                          *
 *   Copyright (C) 2021 by João Vitor Sampaio Ferreira(@Keowu)              *
 *                                                                          *
 *   www.joaovitor.gq                                                       *
 *   www.github.com/keowu                                                   *
 *                                                                          *
 *   Licença de código e uso:                                               *
 *   - Fique mais do que livre para usar o código desde que obedeça a segui *
 *   te regra, e deixe os creditos originais para mim, isso é que deixe     *
 *   um link ou citação para a pesquisa original e deixe um like.           *
 *                                                                          *
 ****************************************************************************/
#pragma once
#include <iostream>
#include <Windows.h>
#include <cstringt.h>

// Macros usadas por mim para agilizar meu trabalho de analise do algoritmo
#define HIBYTE(x)   (*((BYTE*)&(x)+1)) // a Maior parte de um byte ou 4 bits. 1111 0000 
#define LOBYTE(x)   (*((BYTE*)&(x)))   // a Menor parte de um byte ou 4 bits. 0000 1111

// Pegando o byte 1 contando do zero 0000 0010
#define BYTEn(x, n)   (*((BYTE*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)    
// Pegando o byte 2 contando do zero 0000 0100
#define BYTE2(x)   BYTEn(x,  2)
//menor parte de uma dword
#define LODWORD(x)  (*((DWORD*)&(x)))

// Macros da própria VALVE usado para determinar se é um desenvolvedor que está testando o algoritmo
#define STEAM_IS_DEV_STATUS 40;

// Paragmas para ignorar funções da API antigas e permitir que possa-se manter a igualdade de funções originais usada no fonte original
// Original VS8 -> VS19(Atual)
#pragma warning(disable : 4996)
#pragma warning(disable : 4700)
#pragma warning(disable : 28159)
#pragma warning(disable : 6001)

/// <summary>
///		Classe reconstruída para analise e implementação do algoritmo em outros softwares que não sejam seu original
/// </summary>
class tier0_CPUInformation {

	struct CPUInformation
	{
		int	 m_Size;		// é o tamanho dessa estrutura

		bool m_bRDTSC : 1, // A CPU ATUAL CONSEGUE CONTAR O TEMPO ENTRE INSTRUÇÕES
			m_bCMOV : 1,  // ELE SUPORTA A INSTRUÇÃO CMOV ?
			m_bFCMOV : 1,  // ELE SUPORTA A INSTRUÇÃO FCMOV ?
			m_bSSE : 1,	// ELE SUPORTA A INSTRUÇÃO SSE ?
			m_bSSE2 : 1,	// ELE SUPORTA A INSTRUÇÃO SSE2 ?
			m_b3DNow : 1,	// ELE SUPORTA A TECNOLOGIA 3DNow ?
			m_bMMX : 1,	// ELE SUPORTA A TECNOLOGIA MMX ?
			m_bHT : 1;	// ELE SUPORTA A TECNOLOGIA do HyperThreading ?

		UINT8 m_nLogicalProcessors;		// Número de processadores lógicos.
		UINT8 m_nPhysicalProcessors;	// Número de processadores físicos.

	  /*
		Provavelmente é uma verificação de FLAGS, não consegui determinar uma utilidade para isso somente com o código Assembly e Pseudocódigo reconstruído
	  */
		//const m_bSSE3
		bool m_bSSE3 : 1,
			m_bSSSE3 : 1,
			m_bSSE4a : 1,
			m_bSSE41 : 1,
			m_bSSE42 : 1;

		INT64 m_Speed;						//Cliclos por segundo do processador

		TCHAR* m_szProcessorID;				// ID do processador, número de identificação

		CPUInformation() : m_Size(0) {}  // Tamanho da Struct
	};

private:
	int getCPUClocksInfo();
	char verifyCPUBrand();
	char getCPUID(int P1, DWORD* P2, DWORD* P3, DWORD* P4, DWORD* P5);
	char IsWin98OrOlder();
	void isGenericProcessor();
	int  CPUCmp(unsigned __int8* ACMD, const char* STRRELATIVE);
	char verificaCPUCustom();

	CPUInformation pCPUInformationMemory;
	bool globalDbgFlag = 1; // Verificação do 1 & 1 = 1, se for zero é erro.
	int storagelocalGenericProcessor = 0;
	BYTE intelGNI = 0;

public:
	int* GetCPUInformation();

};