#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define TAM_PALA 10
#define TAM_CODIGO 10
#define TAM_CODIFICACAO 256
#define ARQ_TAB "TabFreq.dat"
#define ARQ_CODIGO "Codigo Huffman.txt"

/* 

ALUNO: ITALO CESAR PIOVAN ROCHA

*/

///ESTRUTURAS
struct Tabela
{
	char palavra[TAM_PALA],huffman[TAM_CODIGO];
	int frequencia,simbolo;	
		
};typedef struct Tabela tab;

struct Tree
{
	int simbolo;
	int freq;
	Tree *esq,*dir;  
	
};typedef struct Tree tree;

struct ListaFreq
{
	Tree *no;
	ListaFreq *prox;
};
typedef ListaFreq listafreq;
/// FIM ESTRUTURAS

void init(listafreq **lf,Tree **t)
{
	*t = NULL;
	*lf = NULL;
}

char isEmpty(listafreq *lf)
{
	return lf == NULL;
}

Tree *CriaNO(int simbolo,int freq,Tree *esq,Tree *dir)
{
	Tree *no = (Tree*)malloc(sizeof(Tree)); 
	no->esq = esq;
	no->dir = dir;
	no->freq = freq;
	no->simbolo = simbolo;
	return no;
}

void Insere(listafreq **lf,Tree *no)
{
	listafreq *aux;
	listafreq *nova = (listafreq*)malloc(sizeof(listafreq));
	
	nova->no = no;
	nova->prox = NULL;
	
	if(isEmpty(*lf))
		*lf = nova;
	else
	{
		if(no->freq < (*lf)->no->freq)
		{
			nova->prox = *lf;
			(*lf) = nova;
		}	
		else
		{
			aux = *lf;
			while(aux->prox != NULL && aux->prox->no->freq < no->freq)
				aux = aux->prox;	
			nova->prox = aux->prox;
			aux->prox = nova;
		}	
	}
}

void Remove(listafreq **lf,Tree **no)
{
	listafreq *aux = *lf;
	if(!isEmpty(*lf))
	{
		*no = (*lf)->no;
		*lf =(*lf)->prox;
		free(aux);
	}
} 

int BuscaPalavra(Tabela t[],char pala[],int tam,int simbolo)
{
	int i=0;
	
	if(simbolo == -1)// procura pelo simbolo
		while(i < tam && strcmp(t[i].palavra,pala) != 0)
			i++;
	else // procura pela palavra
		while(i < tam && t[i].simbolo != simbolo)
			i++;
	
	if(!strcmp(t[i].palavra,pala) || simbolo == t[i].simbolo)
		return i;
	else
		return -1;		
}

int getEspaco(char texto[]) // retorna quantidade de espaços, consequentemente a quantidade de palavras. 
{							// usado para inicializar a tabela de frequencia, e aproveitando a frequência de espaço
	int espaco=0;
					
	for(int i=0;i <= strlen(texto);i++)
		if(texto[i] == ' ')
			espaco++;	
	return espaco;
}

void SeparaPalavras(char texto[],Tabela *t,int *TL)
{
	int i=0,letra=0,pos=-1,j=0;

	char palavra[TAM_PALA]="";
	
	while(i <= strlen(texto)) // percorre texto inteiro 
	{
		while(texto[i] != ' ' && i < strlen(texto))
			palavra[letra++] = texto[i++]; // gera palavras
		palavra[letra] = '\0';
		
		pos = BuscaPalavra(t,palavra,j,-1); // busca palavra se ja existe
		if(pos > -1)
			t[pos].frequencia++; // se existir, incrementa frequencia
		else // se não existir, add com frequencia 1
		{
			t[j].frequencia=1;
			t[j].simbolo = j;
			strcpy(t[j].palavra,palavra);
			strcpy(t[j].huffman,"");
			j++;
		}		
		i++;
		letra = 0; strcpy(palavra,""); // "zera" var palavra 
	}
	// add o espaço na tabela de palavras e sua frequência... 
	t[j].frequencia=getEspaco(texto);
	t[j].simbolo = j;
	strcpy(t[j].palavra," ");
	strcpy(t[j].huffman,"");
	j++;
	*TL = j;
}

void GeraTree(Tree **raiz,listafreq **lf)
{
	Tree *esq,*dir,*no;
	*raiz = esq = dir = NULL;
	
	while((*lf)->prox != NULL)
	{
		Remove(&*lf,&dir);
		Remove(&*lf,&esq);
		
		no = CriaNO(-1,esq->freq + dir->freq,esq,dir);
		
		Insere(&*lf,no);
	}
	*raiz = no;
}

void GeraHuffman(Tree *raiz, int tl,char *cod,tab *t,int tam)
{
	if(raiz->esq != NULL && raiz->dir != NULL)
	{
		cod[tl] = '0';cod[tl+1] = '\0';
		GeraHuffman(raiz->esq,tl+1,&*cod,&*t,tam);
		cod[tl] = '1';cod[tl+1] = '\0';
		GeraHuffman(raiz->dir,tl+1,&*cod,&*t,tam);	
	}
	else
		strcpy(t[BuscaPalavra(t,"",tam,raiz->simbolo)].huffman,cod);		
}

void GravaArquivoTab(Tabela t[],int TL)
{
	FILE *ptr = fopen(ARQ_TAB,"wb");
	
	for(int i=0;i< TL ;i++)
		fwrite(&t[i],sizeof(Tabela),1,ptr);
	fclose(ptr);
}

void GravaArquivoCod(Tabela t[],int TL,char texto[])
{
	int i =0,letra =0;
	char palavra[TAM_PALA]="",codigo[TAM_CODIFICACAO]="";
	FILE *ptr = fopen(ARQ_CODIGO,"w");
	
	while(i <= strlen(texto))
	{
		while(texto[i] != ' ' && i < strlen(texto))
			palavra[letra++] = texto[i++]; 
		palavra[letra] = '\0';
		 		
		strcat(codigo,t[BuscaPalavra(t,palavra,TL,-1)].huffman);
		if(texto[i] == ' ')
			strcat(codigo,t[BuscaPalavra(t," ",TL,-1)].huffman);
		i++; letra = 0; strcpy(palavra,"");
	}
	fputs(codigo,ptr);
	
	fclose(ptr);	
}

void Codifica(Tabela *t,int TL,char texto[])
{
	listafreq *lf; 
	Tree *raiz;
	char codigo[TAM_CODIGO];
	
	init(&lf,&raiz);
	
	char pala[TAM_PALA];
	int freq;
	
	for(int i=0;i< TL ;i++)
		Insere(&lf,CriaNO(t[i].simbolo,t[i].frequencia,NULL,NULL));
	
	GeraTree(&raiz,&lf);
	GeraHuffman(raiz,0,codigo,t,TL);
	GravaArquivoTab(t,TL);
	GravaArquivoCod(t,TL,texto);
}

int main()
{
	int TL=0;
	char texto[] = "eu quero amor eu quero alegria eu quero calor eu quero fantasia";
	//"amo como ama o amor nao conheco nenhuma outra razao para amar senao amar que queres que te diga alem de que te amo se o que quero dizer te e que te amo";
	//"vi uma estrela tao alta vi uma estrela tao fria vi uma estrela luzindo na minha vida vazia era uma estrela tao alta era uma estrela tao fria era uma estrela sozinha luzindo no fim do dia";
	
	Tabela t[getEspaco(texto)];
	
	SeparaPalavras(texto,t,&TL);
	Codifica(t,TL,texto);
}
