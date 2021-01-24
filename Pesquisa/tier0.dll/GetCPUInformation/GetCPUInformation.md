<h1 align="center">Tier0.dll | GetCPUInformation</h1>
<h2 align="center">Resumo da analise da função exportada pela DLL</h2>

<p align="center">Somente possui duas menções publicas online, sendo sobre sua existencia em um código fonte para xbox vazado na internet e a outra na steam SDK Repo </p>
<h3 align="center">Capacidades do algoritimo exportado: </h3>
<ul align="center">
  <li>m_szProcessorID("Verifica o ID do processador")</li>
  <li>cpu.m_Speed("Verifica a velocidade máxima que o processador consegue atingir")</li>
  <li>m_nPhysicalProcessors("Verifica os números de processadores físicos")</li>
</ul>
<h3 align="center">Composição do struct em memória:</h3>

```c++
struct CPUInformation
{
	int	 m_Size;		// é o tamanho dessa estrutura

	bool m_bRDTSC : 1, // A CPU ATUAL CONSEGUE CONTAR O TEMPO ENTRE INSTRUÇÕES
		 m_bCMOV  : 1,  // ELE SUPORTA A INSTRUÇÃO CMOV ?
		 m_bFCMOV : 1,  // ELE SUPORTA A INSTRUÇÃO FCMOV ?
		 m_bSSE	  : 1,	// ELE SUPORTA A INSTRUÇÃO SSE ?
		 m_bSSE2  : 1,	// ELE SUPORTA A INSTRUÇÃO SSE2 ?
		 m_b3DNow : 1,	// ELE SUPORTA A TECNOLOGIA 3DNow ?
		 m_bMMX   : 1,	// ELE SUPORTA A TECNOLOGIA MMX ?
		 m_bHT	  : 1;	// ELE SUPORTA A TECNOLOGIA do HyperThreading ?

	UINT8 m_nLogicalProcessors;		// Número de processadores lógicos.
	UINT8 m_nPhysicalProcessors;	// Número de processadores físicos.
	
  /*
    Provavelmente é uma verificação de FLAGS, não consegui determinar uma utilidade para isso somente com o código Assembly e Pseudocódigo reconstruído
  */
	bool m_bSSE3 : 1, 
		 m_bSSSE3 : 1,
		 m_bSSE4a : 1,
		 m_bSSE41 : 1,
		 m_bSSE42 : 1;	

	INT64 m_Speed;						//Cliclos por segundo do processador // In cycles per second.

	TCHAR* m_szProcessorID;				// ID do processador, número de identificação

	CPUInformation(): m_Size(0){}  // Tamanho da Struct
};
```

<h3 align="center"> Exemplos de chamada da função exportada: </h3>
<p align="center">Somente é possível acessar essa DLL e esse export a partir de uma injeção direta de código na memória do processo, isso é, carregar um modulo e a partir disso acessar o export.</p>

```c++
  const CPUInformation &cpu = *GetCPUInformation();

````

<h3 align="center">Calculos para determinar a velocidade da CPU</h3>
<p align="center">O Seguinte calculo é usado para determinar a valocidade do processador(Segundo os vazamentos), essa calculo foi comprovado durante os testes:</p>

```c++
float CPU_SPEED = m_Speed*(1.0 / 1.0e9);

```
<h3 align="center">Resultado da analise do código(Não reconstruído totalmente pois muitos dados foram otimizados e dessa forma abstraídos):</h3>
<p align="center">Projeto no mesmo repositório.</p>

<h3 align="center">Banco de dados do IDA:</h3>
<p align="center">O Banco da minha analise está no mesmo repositório, junto com a DLL original.</p>

<img src="https://raw.githubusercontent.com/keowu/CSSourceResearchs/main/Pesquisa/tier0.dll/GetCPUInformation/Screenshots/IDA_1.PNG?token=AKC4ZNSROMZMSMX7OSA7LVDAC3KAQ" alt="Print do Disassembler">
<img src="https://raw.githubusercontent.com/keowu/CSSourceResearchs/main/Pesquisa/tier0.dll/GetCPUInformation/Screenshots/IDA_2.PNG?token=AKC4ZNT7GUAEBZY6KSCNC6DAC3KB6" alt="Print do Meu Pseudocódigo">

<h3 align="center">Veja o funcionamento e implementação do algoritmo no meu projeto de teste:</h3>
<img src="https://raw.githubusercontent.com/keowu/CSSourceResearchs/main/Pesquisa/tier0.dll/GetCPUInformation/Screenshots/working%3DD.gif?token=AKC4ZNRWBSMBH5RIYHQ2WEDAC3KHO" alt="Algoritmo Implementado">

<h4 align="center">Analise por João Ferreira(@Keowu) para suas pesquisas pessoais/estudos.</h4>
