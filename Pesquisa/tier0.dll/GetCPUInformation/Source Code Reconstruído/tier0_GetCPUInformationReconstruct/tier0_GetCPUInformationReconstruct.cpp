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
#include "tier0_GetCPUInformationReconstruct.h"

/// <summary>
///     Determina caso um processador seja diferente do esperado, e determina sua velocidade
/// </summary>
/// <returns>A velocidade do processador</returns>
char tier0_CPUInformation::verificaCPUCustom()
{
    int resultado;
    unsigned int resultadoSHL;
    int CPUID;

    if (this->getCPUID(0x80000000, (DWORD*)&resultadoSHL, (DWORD*)&CPUID, (DWORD*)&CPUID, (DWORD*)&CPUID)
        && resultadoSHL > 0x80000000
        && getCPUID(-2147483647, (DWORD*)&CPUID, (DWORD*)&CPUID, (DWORD*)&CPUID, (DWORD*)&resultadoSHL))
    {
        return resultadoSHL >> 31;
    }
    LOBYTE(resultado) = 0;
    return resultado;
}

/// <summary>
///     Converte e compara o equivalente de um brand const char* para sua conversão UINT8
/// </summary>
/// <param name="ACMD">Ponteiro para a variável que vai armazenar o novo valor</param>
/// <param name="STRRELATIVE">String relativa para trabalhar</param>
/// <returns>O Valor do armazenamento local</returns>
int tier0_CPUInformation::CPUCmp(unsigned __int8* ACMD, const char* STRRELATIVE)
{
    int localStore;
    int plocalsotre;

    do
    {
        while (1)
        {
            localStore = *ACMD;
            plocalsotre = *(unsigned __int8*)STRRELATIVE;
            if (localStore == plocalsotre)
            {
                if (!*ACMD)
                    return 0;
            }
            else
            {
                if (!*STRRELATIVE)
                    return localStore;
                if ((unsigned int)(localStore - 65) <= 0x19)
                    localStore += 32;
                if ((unsigned int)(plocalsotre - 65) <= 0x19)
                    plocalsotre += 32;
                if (localStore != plocalsotre)
                    goto LABEL_19;
            }
            localStore = ACMD[1];
            ACMD += 2;
            plocalsotre = *((unsigned __int8*)STRRELATIVE + 1);
            STRRELATIVE += 2;
            if (localStore != plocalsotre)
                break;
            if (!localStore)
                return 0;
        }
        if (!plocalsotre)
            return localStore;
        if ((unsigned int)(localStore - 65) <= 0x19)
            localStore += 32;
        if ((unsigned int)(plocalsotre - 65) <= 0x19)
            plocalsotre += 32;
    } while (localStore == plocalsotre);
LABEL_19:
    localStore -= plocalsotre;
    return localStore;
}

/// <summary>
///     Determina se o processador é um generico x86 com capatibilidade IA-32 ou algo diferente
/// </summary>
void tier0_CPUInformation::isGenericProcessor()
{
    int CPUID_Y;
    int CPUID_A;
    int CPUID_Z;
    int CPUID_X;

    storagelocalGenericProcessor = 0;
    pCPUInformationMemory.m_bMMX = 0;
    pCPUInformationMemory.m_bHT = 0;
    pCPUInformationMemory.m_bSSE3 = 0;
    if (getCPUID(0, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Y, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_A))
    {
        storagelocalGenericProcessor = CPUID_Y;
        pCPUInformationMemory.m_bMMX = CPUID_A;
        pCPUInformationMemory.m_bHT = CPUID_Z;
    }
    else
    {
        strcpy((char*)&storagelocalGenericProcessor, "Generic_x86");
    }
}

/// <summary>
///     Verifica se a versão do Windows é 98 ou mais antiga ainda
/// </summary>
/// <returns>
///     1 ou '\x01' para caso o jogo esteja sendo executado em um ambiente antigo de windows 98
///     0 ou '\x00' para caso o jogo não esteja sendo executado em um ambiente antigo de windows 98
/// </returns>
char tier0_CPUInformation::IsWin98OrOlder()
{
    char resultado;
    char errorMSG = ' ';
    struct _OSVERSIONINFOA VersionInformation;

    resultado = 0;
    memset(&VersionInformation, 0, 0x9Cu);
    VersionInformation.dwOSVersionInfoSize = 156;
    if (!GetVersionExA(&VersionInformation))
    {
        VersionInformation.dwOSVersionInfoSize = 148;
        if (!GetVersionExA(&VersionInformation))
        {
            //Error("IsWin98OrOlder:  Unable to get OS version information", errorMSG); Função de erro para o console do jogo
            std::cerr << "IsWin98OrOlder:  Unable to get OS version information" << errorMSG << std::endl;
            return '\x00';
            //JUMPOUT(0x10003309); // salto para um base específico
        }
    }
    if (!VersionInformation.dwPlatformId || VersionInformation.dwPlatformId == 1)
        resultado = 1;
    return resultado;
}

/// <summary>
///     Essa função usa uma instrução Assembly inline muito conhecida
///     por meio da syscall cpuid é possível obter diversos detalhes do processador hospedeiro
///     essas informações são armazenadas em registradores EAX, EBX, ECX E EDX
///     Os valores trabalhados são DWORD e são alimentados após a execução
/// 
///     push ARG1       ; Pusha para a pilha o valor do argumento
///     call cpuid      ; Call para CPUID
///     mov P2, EAX     ; Retorno 1
///     mov P3, EBX     ; Retorno 2
///     mov P4, ECX     ; Retorno 3
///     mov P5, EDX     ; Retorno 4
/// 
/// </summary>
/// <param name="P1">Argumento para a CPUID</param>
/// <param name="P2">Ponteiro para EAX</param>
/// <param name="P3">Ponteiro para EBX</param>
/// <param name="P4">Ponteiro para ECX</param>
/// <param name="P5">Ponteiro para EDX</param>
/// <returns>Verdadeiro em char 1 ou '\x01'</returns>
char tier0_CPUInformation::getCPUID(int P1, DWORD* P2, DWORD* P3, DWORD* P4, DWORD* P5)
{
    /*
    * IMPLEMENTAÇÃO DO RDTSC EM ASSEMBLY ORIGINAL
    _EAX = P1;
    __asm { cpuid }
    *P2 = _EAX;
    *P3 = _EBX;
    *P4 = _ECX;
    *P5 = _EDX;
    */
    return 1;
}

/// <summary>
///     Verifica informações de vendor da CPU ou CPU Brand
/// </summary>
/// <returns>O Deslocamento lógico direito ou a parte baixa do valor do RDTSC obtido pela CPU ID</returns>
char tier0_CPUInformation::verifyCPUBrand()
{
    int resultado;
    int CPUID_Y;
    int CPUID_A;
    int CPUID_Z;
    unsigned int CPUID_C;
    int CPUID_B;
    int CPUID_X;

    CPUID_B = 0;
    CPUID_C = 0;
    CPUID_Y = 0;
    CPUID_A = 0;
    CPUID_Z = 0;
    if (this->getCPUID(0, (DWORD *)&CPUID_X, (DWORD*)&CPUID_Y, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_A)
        && this->getCPUID(1, (DWORD*)&CPUID_B, (DWORD*)&CPUID_X, (DWORD*)&CPUID_X, (DWORD*)&CPUID_C)
        && ((CPUID_B & 0xF00) == 3840 || (CPUID_B & 0xF00000) != 0)
        && CPUID_Y == 1970169159
        && CPUID_A == 1231384169
        && CPUID_Z == 1818588270)
    {
        return (CPUID_C >> 28) & 1;
    }
    LOBYTE(resultado) = 0;
    return resultado;
}


/// <summary>
///     Obtem informações da frequência da CPU e seus Clocks
/// </summary>
/// <returns>A cpuFrequence</returns>
int tier0_CPUInformation::getCPUClocksInfo()
{
    unsigned __int64 rdtscRET;
    unsigned __int64 cpuFrequence;
    int resultado;
    LARGE_INTEGER PerformanceCount;
    LARGE_INTEGER lpPerformanceCount;
    LARGE_INTEGER lpFrequency;

    QueryPerformanceFrequency(&lpFrequency);
    lpFrequency.QuadPart >>= 5;
    QueryPerformanceCounter(&PerformanceCount);
    rdtscRET = __rdtsc();
    do
        QueryPerformanceCounter(&lpPerformanceCount);
    while (lpPerformanceCount.QuadPart - PerformanceCount.QuadPart < lpFrequency.QuadPart);
    cpuFrequence = 32 * __rdtsc() - 32 * rdtscRET;
    if (cpuFrequence)
        resultado = cpuFrequence;
    else
        resultado = 2000000000;
    return resultado;
}


/// <summary>
///     Esse algoritmo é usado pela valve para obter as seguintes informações:
///     Verificar o ID do Processador
///     Verifica a velocidade máxima que o processador consegue atingir
///     Verifica os números de processadores físicos
///     Verifica os números de processadores Lógicos
///     Se o processador suporta tecnologias como:
///          - CMOV 
///          - FCMOV
///          - SSE
///          - SSE2
///          - 3DNow
///          - MMX
///          - HyperThreading
/// </summary>
/// <returns>Ponteiro para a estrutura alocada na memória, é feito uma troca de ponteiros na memória.</returns>
int* tier0_CPUInformation::GetCPUInformation()
{
    __int64 LoCpu;
    unsigned int HICPUID_Y;
    char SysProcessorsCTN;
    char GNIctn;
    bool flagCPU;
    bool flagCPU1;
    char flagCPU2;
    char flagCPU3;
    bool flagCPU4;
    bool flagCPU5;
    char flagCPU6;
    bool flagCPU7;
    unsigned __int8* ACMD = 0;
    bool flagCPU8;
    bool flagCPU9;
    unsigned __int8* GINTEL = 0;
    bool isIntelCPU;
    int dwProcessorType;
    struct _SYSTEM_INFO SystemInfo;
    int CPUID_C;
    int CPUID_Z;
    unsigned int CPUID_Y;
    int CPUID_X;

    if ((globalDbgFlag & 1) != 0)
    {
        // Usada para debbug de desenvolvedores
        /*
        if (pCPUInformationMemory == STEAM_IS_DEV_STATUS)
            return (int *)&pCPUInformationMemory;
        */
    }
    else
    {
        globalDbgFlag |= 1u;
    }
    intelGNI = 0;
    pCPUInformationMemory.m_bRDTSC = 0i64;
    pCPUInformationMemory.m_szProcessorID;
    pCPUInformationMemory.m_bCMOV = 0;
    pCPUInformationMemory.m_bFCMOV = 0;
    pCPUInformationMemory.m_bSSE = 0;
    pCPUInformationMemory.m_bSSE2 = 0;
    //pCPUInformationMemory = 40;
    LODWORD(LoCpu) = this->getCPUClocksInfo();
    pCPUInformationMemory.m_bRDTSC = LoCpu;
    CPUID_Y = 0;
    if (this->verifyCPUBrand())
    {
        if (getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Y, (DWORD*)&CPUID_X, (DWORD*)&CPUID_X))
            HICPUID_Y = HIWORD(CPUID_Y);
        else
            LOBYTE(HICPUID_Y) = 1;
    }
    else
    {
        LOBYTE(HICPUID_Y) = 1;
    }
    BYTE1(intelGNI) = HICPUID_Y;
    SystemInfo.dwOemId = 0;
    SystemInfo.dwPageSize = 0;
    SystemInfo.lpMinimumApplicationAddress = 0;
    SystemInfo.lpMaximumApplicationAddress = 0;
    SystemInfo.dwActiveProcessorMask = 0;
    SystemInfo.dwNumberOfProcessors = 0;
    SystemInfo.dwProcessorType = 0;
    SystemInfo.dwAllocationGranularity = 0;
    *(DWORD*)&SystemInfo.wProcessorLevel = 0;
    GetSystemInfo(&SystemInfo);
    SysProcessorsCTN = SystemInfo.dwNumberOfProcessors / BYTE1(intelGNI);
    GNIctn = BYTE1(intelGNI) * SysProcessorsCTN;
    BYTE2(intelGNI) = SysProcessorsCTN;
    BYTE1(intelGNI) *= SysProcessorsCTN;
    if (!SysProcessorsCTN && !GNIctn)
        *(WORD*)((char*)&intelGNI + 1) = 257;
    if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_C, (DWORD*)&CPUID_Y))
        flagCPU = (CPUID_Y & 0x10) != 0;
    else
        flagCPU = 0;
    LOBYTE(intelGNI) = (flagCPU ^ intelGNI) & 1 ^ intelGNI;
    if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_C, (DWORD*)&CPUID_Y))
        flagCPU1 = (CPUID_Y & 0x8000) != 0;
    else
        flagCPU1 = 0;
    LOBYTE(intelGNI) = (intelGNI ^ (2 * flagCPU1)) & 2 ^ intelGNI;
    if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_C, (DWORD*)&CPUID_Y))
        flagCPU2 = BYTE2(CPUID_Y) & 1;
    else
        flagCPU2 = 0;
    LOBYTE(intelGNI) = (intelGNI ^ (4 * flagCPU2)) & 4 ^ intelGNI;
    if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_C, (DWORD*)&CPUID_Y))
        flagCPU3 = (CPUID_Y & 0x800000) != 0;
    else
        flagCPU3 = 0;
    LOBYTE(intelGNI) = (intelGNI ^ (flagCPU3 << 6)) & 0x40 ^ intelGNI;
    if ((unsigned __int8)this->IsWin98OrOlder())
    {
        flagCPU4 = 0;
    }
    else if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_C, (DWORD*)&CPUID_Y))
    {
        flagCPU4 = (CPUID_Y & 0x2000000) != 0;
    }
    else
    {
        flagCPU4 = 0;
    }
    LOBYTE(intelGNI) = (intelGNI ^ (8 * flagCPU4)) & 8 ^ intelGNI;
    if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_C, (DWORD*)&CPUID_Y))
        flagCPU5 = (CPUID_Y & 0x4000000) != 0;
    else
        flagCPU5 = 0;
    LOBYTE(intelGNI) = (intelGNI ^ (16 * flagCPU5)) & 0x10 ^ intelGNI;
    if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_Y, (DWORD*)&CPUID_C))
        flagCPU6 = CPUID_Y & 1;
    else
        flagCPU6 = 0;
    HIBYTE(intelGNI) ^= (HIBYTE(intelGNI) ^ flagCPU6) & 1;
    if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_Y, (DWORD*)&CPUID_C))
        flagCPU7 = (CPUID_Y & 0x200) != 0;
    else
        flagCPU7 = 0;
    HIBYTE(intelGNI) ^= (HIBYTE(intelGNI) ^ (2 * flagCPU7)) & 2;
    //this->isGenericProcessor(); <- Presente, mas não usado no código original, mas o disassembler e o depurador nunca usam esta função.
    if (this->CPUCmp(ACMD, "AuthenticAMD"))
    {
        flagCPU8 = 0;
    }
    else if (this->getCPUID(-2147483647, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_Y, (DWORD*)&CPUID_C))
    {
        flagCPU8 = (CPUID_Y & 0x40) != 0;
    }
    else
    {
        flagCPU8 = 0;
    }
    HIBYTE(intelGNI) ^= (HIBYTE(intelGNI) ^ (4 * flagCPU8)) & 4;
    if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_Y, (DWORD*)&CPUID_C))
        flagCPU9 = (CPUID_Y & 0x80000) != 0;
    else
        flagCPU9 = 0;
    HIBYTE(intelGNI) ^= (HIBYTE(intelGNI) ^ (8 * flagCPU9)) & 8;
    //this->isGenericProcessor(); <- Presente, mas não usado no código original, mas o disassembler e o depurador nunca usam esta função.
    if (this->CPUCmp(GINTEL, "GenuineIntel"))
    {
        isIntelCPU = 0;
    }
    else if (this->getCPUID(1, (DWORD*)&CPUID_X, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_Y, (DWORD*)&CPUID_C))
    {
        isIntelCPU = (CPUID_Y & 0x100000) != 0;
    }
    else
    {
        isIntelCPU = 0;
    }
    HIBYTE(intelGNI) ^= (HIBYTE(intelGNI) ^ (16 * isIntelCPU)) & 0x10;
    LOBYTE(intelGNI) = (intelGNI ^ (32 * this->verificaCPUCustom())) & 0x20 ^ intelGNI;
    //this->isGenericProcessor(); <- Presente, mas não usado no código original, mas o disassembler e o depurador nunca usam esta função.
    pCPUInformationMemory.m_szProcessorID = (TCHAR *)dwProcessorType;
    LOBYTE(intelGNI) = intelGNI & 0x7F | (verifyCPUBrand() << 7);
    if (this->getCPUID(1, (DWORD*)&CPUID_Y, (DWORD*)&CPUID_C, (DWORD*)&CPUID_Z, (DWORD*)&CPUID_X))
    {
        pCPUInformationMemory.m_bCMOV = CPUID_Y;
        pCPUInformationMemory.m_bFCMOV = CPUID_X;
        pCPUInformationMemory.m_bSSE = CPUID_Z;
        pCPUInformationMemory.m_bSSE2 = CPUID_C;
    }
    return (int *)&pCPUInformationMemory;
}

/// <summary>
///     Main
/// </summary>
/// <returns>Não retorna nada</returns>
int main()
{
    std::cout << "Hello World!\n";
}