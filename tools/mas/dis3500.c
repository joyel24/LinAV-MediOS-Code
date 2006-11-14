#include <stdio.h>

#include "code\D1_800_435.h"

char FLAGS[32][6]={"FALSE","TRUE","CC","CS","P","N","GE","LT","GT","LE","AC","AS","BC","BS","MC","MS","SC","SS","PSC","PSS","GC","GS","XC","XS","YC","YS","RC","RS","QC","QS","NE","EQ"};
char SHIFTS[4][9]={"","<<1",">>1","!ERROR!"};

char OPERATORS[16][5]={"ADD","ADC","SUB","SBC","IAD","IAC","<<","SMP","ASH","EXP","NOR","AND","XOR","NEG1","ABS1","ABS0"};
int OPERANDS[16]={3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,1}; // 1 - D0, 2 - D1, 3 both

int MAC1[8]={0,0,0,1,1,1,-1,-1};
char MAC2[8][5]={"","-","","+!","+","-","+","-"};

void Flag(long int Code)
{
 long int Type;
 Type=(Code&0x1F00)>>8;
 printf("%s",FLAGS[Type]);
} 

/* JUMP instructions */

void JMP0000(long int Code)
{
 long int Type;
 Type=(Code&0xC0)>>6;
 switch (Type) {
      case 0: printf("NOP");
              break;
      case 1: printf("TASK");
              break;
      default: {
                printf("CLEAR MPS");
                if (Type==3) printf(",TASK"); 
               }     
              break; 
 }
} 
 
void JMP0001(long int Code)
{
 long int Type;
 Type=(Code&0x40)>>6;
 printf("HALT");
 if (Type) printf(",TASK");
} 

void JMP0010(long int Code)
{
 long int Type;
 Type=(Code&0x3FFF);
 printf("IF (");
 Flag(Type);
 printf(") DPC=START,J%ld",Type&0x07);
 if (Type&0x80) printf(",SKIP");
}
 
void JMP0011(long int Code)
{
 long int Type;
 Type=(Code&0x3FFF);
 printf("IF (");
 Flag(Type);
 printf(") DPC=J%ld",Type&0x07);
 if (Type&0x80) printf(",SKIP");
}

void JMP0100(long int Code)
{
 long int Type;
 Type=(Code&0x3FFF);
 printf("START=YPC,HARDLOOPS=%ld,LEN=%ld",Type&0x7F,(Type>>7)&0x7F);
}

void JMP0101(long int Code)
{
 long int Type;
 Type=(Code&0x3FFF);
 printf("START=YPC,HARDLOOPS=LC,LEN=%ld",(Type>>7)&0x7F);
}

void JMP0110(long int Code)
{
 long int Type;
 Type=(Code&0x3FFF);
 printf("IF (");
 Flag(Type);
 printf(") DPC=SCHEDULE");
 if (Type&0x80) printf(",SKIP");
 printf(",CPC=J%",(Type>>3)&7);
 if (Type&0x40) printf(",TASK");
}

void JMP0111(long int Code)
{
 long int Type;
 Type=(Code&0x3FFF);
 printf("IF (");
 Flag(Type);
 printf(") DPC=J%ld",Type&7);
 if (Type&0x80) printf(",SKIP");
 printf(",CPC=J%",(Type>>3)&7);
 if (Type&0x40) printf(",TASK");
}

void JMP1000(long int Code)
{
 long int Type;
 Type=(Code&0x3FFF);
 printf("DPC=%ld",Type);
}

void JMP1001(long int Code)
{
 long int Type;
 Type=(Code&0x3FFF);
 printf("VECTOR %ld",Type);
}

void JMP1010(long int Code)
{
 long int Type;
 Type=(Code&0x3FFF);
 printf("IF (");
 Flag(Type);
 printf(") SKIP %ld",(Type>>3)&31);
}

void JMP1011(long int Code)
{
 long int Type;
 printf("VECTOR LC",Type);
}

/* Jump main */

void SolveJMP(long int Code)
{
 long int Type;
 Type=(Code&0x3C000)>>14;
 switch(Type) { 
      case 0: JMP0000(Code);
              break;
      case 1: JMP0001(Code);
              break;
      case 2: JMP0010(Code);
              break;
      case 3: JMP0011(Code);
              break;
      case 4: JMP0100(Code);
              break;
      case 5: JMP0101(Code);
              break;
      case 6: JMP0110(Code);
              break;
      case 7: JMP0111(Code);
              break;
      case 8: JMP1000(Code);
              break;
      case 9: JMP1001(Code);
              break;
      case 10: JMP1010(Code);
               break;
      case 11: JMP1011(Code);
               break;
      default: printf("(UNKNOWN INSTRUCTION)");
 }               
} 
 
void MOV00xx(long int Code)
{
 long int Type,MA,AC,P,S,I,L;
 Type=Code&0xFFFF;
 AC=(Type>>11)&7;
 MA=(Type>>15)&1;
 I=(Type>>3)&31;
 L=(Type>>14)&1;
 if (MA) {P=Type&3;S=(Type>>2)&1;}
    else {P=(Type>>8)&3;S=(Type>>10)&1;}
 printf("CC%ld=Z%ld%ld",AC,MA,P);
 if (S) printf("[%ld]",I);
 if (L) printf(" (L)");
}  

void MOV01xx(long int Code)
{
 long int Type,AC,P[2],S[2],MV,I,L;
 Type=Code&0xFFFF;
 AC=(Type>>11)&7;
 MV=(Type>>15)&1;
 I=(Type>>3)&31;
 L=(Type>>14)&1;
 P[1]=Type&3;S[1]=(Type>>2)&1;
 P[0]=(Type>>8)&3;S[0]=(Type>>10)&1;
 printf("Z%ld%ld",1-MV,P[1-MV]);
 if (S[1-MV]) printf("[%ld]",I);
 printf("=YC%ld, CC%ld=Z%ld%ld",AC,AC,MV,P[MV]);
 if (S[MV]) printf("[%ld]",I);
 if (L) printf(" (L)");
}  

void MOV1000(long int Code)
{
 long int Type,AC,I,R;
 Type=Code&0xFFFF;
 AC=(Type>>11)&7;
 R=Type&0x7f;
 I=(Type>>10)&1;
 printf("CC%ld=ZR%ld(%ld)",AC,I,R);
}  

void MOV1001(long int Code)
{
 long int Type,P[2],L[2],R;
 Type=Code&0xFFFF;
 P[0]=(Type>>8)&3;L[0]=(Type>>7)&1;
 P[1]=(Type>>11)&3;L[1]=(Type>>10)&1;
 R=Type&0x7f;
 if (L[0]) printf("ZR0(%ld)=Z0%ld",R,P[0]);
 if (L[0]&L[1]) printf(" , ");
 if (L[1]) printf("ZR1(%ld)=Z1%ld",R,P[1]);
}  

void MOV101x(long int Code)
{
 long int Type,MA,AC,P,S,L,R,I;
 Type=Code&0xFFFF;
 AC=(Type>>11)&7;
 S=(Type>>8)&3;
 I=(Type>>10)&1;
 R=Type&0x7f;
 L=(Type>>14)&1;
 printf("ZR%ld(%ld)=YC%ld",I,R,AC);
 switch (S) {
      case 1: printf("[<<1]");
              break;
      case 2: printf("[>>1]");
              break;
      case 3: printf("[!ERROR!]");
              break;
 }             
 if (L) printf(" (L)");
}  

void MOV1100(long int Code)
{
 long int Type,AC,M,I;
 Type=Code&0xFFFF;
 AC=(Type>>11)&7;
 I=(Type>>10)&1;
 M=Type&0x3ff;
 printf("CL%ld=$%04X (D%ld)",AC,M,I);
}  

void MOV1101(long int Code)
{
 long int Type,AC,I,M;
 Type=Code&0xFFFF;
 AC=(Type>>11)&7;
 I=(Type>>10)&1;
 M=(Type&0x3ff);
 printf("CH%ld=$%04X (D%ld)",AC,M,I);
}  

void MOV111x(long int Code)
{
 long int Type,AC,P,S,L,I,IS,M;
 Type=Code&0xFFFF;
 AC=(Type>>11)&7;
 S=(Type>>8)&3;
 P=Type&3;
 M=(Type>>10)&1;
 I=(Type>>3)&31;
 IS=(Type>>2)&1;
 L=(Type>>14)&1;
 printf("Z%ld%ld",M,P);
 if (IS) printf("[%ld]",I);
 printf("=YC%ld",AC);
 printf(" %s",SHIFTS[IS]);
 if (L) printf(" (L)");
}  


void SolveMOV(long int Code)
{
 long int Type;
 Type=(Code&0x3C000)>>14;
 switch(Type) {
      case 0: 
      case 1:
      case 2:
      case 3: MOV00xx(Code);
              break;
      case 4: 
      case 5: 
      case 6: 
      case 7: MOV01xx(Code);
              break;
      case 8: MOV1000(Code);
              break;
      case 9: MOV1001(Code);
              break;
      case 10: 
      case 11: MOV101x(Code);
               break;
      case 12: MOV1100(Code);
               break;
      case 13: MOV1101(Code);
               break;
      case 14:
      case 15: MOV111x(Code);
               break;
 }               
} 

void ALU00(long int Code)
{
 long int Type,ALU,AC,P[2],IS[2],I,CM;
 Type=Code&0x3FFFF;
 P[0]=(Type>>8)&3;P[1]=Type&3;
 IS[0]=(Type>>10)&1;IS[1]=(Type>>2)&1;
 I=(Type>>3)&7;
 AC=(Type>>11)&7;
 ALU=(Type>>14)&15;
 printf("CC%ld=",AC);
 if (OPERANDS[ALU]==3) { 
      printf("Z0%ld",P[0]);
      if (IS[0]) printf("[%ld]",I);
      printf(" ");
 }
 printf("%s ",OPERATORS[ALU]);
 CM=OPERANDS[ALU]>>1;
 printf("Z%ld%ld",CM,P[CM]);
 if (IS[CM]) printf("[%ld]",I);
} 

void ALU01(long int Code)
{
 long int Type,ALU,AC,AS,I;
 char S[2]={0,0};
 Type=Code&0x3FFFF;
 I=Type&0x3F;
 if (I&32) {I=(I^0x3F)+1;S[0]='-';} // 2 Complement
 AC=(Type>>11)&7;
 AS=(Type>>8)&7;
 ALU=(Type>>14)&15;
 printf("CC%ld=",AC);
 if (OPERANDS[ALU]==3) printf("AC%ld ",AS);
 printf("%s ",OPERATORS[ALU]);
 if (OPERANDS[ALU]==1) printf("AC%ld ",AS);
                  else printf("%s%ld",S,I);
} 


void ALU10(long int Code)
{
 long int Type,ALU,BS,P,IS,I,AC;
 Type=Code&0x3FFFF;
 I=(Type>>3)&7;
 AC=(Type>>11)&7;
 BS=Type&7;
 P=(Type>>8)&3;
 IS=(Type>>10)&1;
 ALU=(Type>>14)&15;
 printf("CC%ld=",AC);
 if (OPERANDS[ALU]==3) {
      printf("Z0%ld",P);
      if (IS) printf("[%ld]",I);
      printf(" ");
 }
 printf("%s ",OPERATORS[ALU]);
 if (OPERANDS[ALU]==1) {
      printf("Z0%ld ",P);
      if (IS) printf("[%ld]",I);
      }     
 else printf("AC%ld",BS);
} 

void ALU11(long int Code)
{
 long int Type,ALU,AC,AS,BS;
 Type=Code&0x3FFFF;
 BS=Type&7;
 AS=(Type>>8)&7;
 AC=(Type>>11)&7; 
 ALU=(Type>>14)&15;
 printf("CC%ld=",AC);
 if (OPERANDS[ALU]==3) printf("AC%ld ",AS);
 printf("%s ",OPERATORS[ALU]);
 if (OPERANDS[ALU]==1) printf("AC%ld",AS);
                  else printf("AC%ld",BS);
} 


void SolveALU(long int Code)
{
 long int Type;
 Type=(Code&0xC0)>>6;
 switch(Type) {
      case 0: ALU00(Code);
              break; 
      case 1: ALU01(Code);
              break;
      case 2: ALU10(Code);
              break;
      case 3: ALU11(Code);
              break;
 }               
} 

void MAC0000(long int Code)
{
 long int Type,AC,CHK;
 Type=Code&0xFFFF;
 AC=(Type>>11)&7; 
 CHK=(Type>>15)&7;
 if (CHK==2) printf("FC%ld=LMUL",AC);
        else printf("(UNKNOWN INSTRUCTION");
} 

void MAC0001(long int Code)
{
 long int Type,AC,BS,P,MD,M;
 Type=Code&0x3FFFF;
 BS=Type&7;
 P=(Type>>8)&3;
 MD=(Type>>10)&1;
 if (MD) {
          printf("UNKNOWN INSTRUCTION"); 
          return;
 }     
 AC=(Type>>11)&7; 
 M=(Type>>15)&7;
 printf("FC%ld=",AC);
 if (MAC1[M]) {
       if (MAC1[M]<0) printf("-");
       printf("-DC%ld",AC);
 } 
 printf("%s",MAC2[M]);
 printf("Z0%ld*AC%ld",P,BS);
} 

void MAC0010(long int Code)
{
 long int Type,AC,AS,P,MD,M;
 Type=Code&0x3FFFF;
 P=Type&3;
 AS=(Type>>8)&7;
 MD=(Type>>2)&1;
 if (MD) {
          printf("UNKNOWN INSTRUCTION"); 
          return;
 }     
 AC=(Type>>11)&7; 
 M=(Type>>15)&7;
 printf("FC%ld=",AC);
 if (MAC1[M]) {
       if (MAC1[M]<0) printf("-");
       printf("DC%ld",AC);
 } 
 printf("%s",MAC2[M]);
 printf("AC%ld*Z1%ld",AS,P);
} 

void MAC0011(long int Code)
{
 long int Type,AC,BS,S,P,MD,M;
 Type=Code&0x3FFFF;
 BS=Type&7;
 S=(Type>>6)&3;
 P=(Type>>8)&3;
 MD=(Type>>10)&1;
 if (MD) {
          printf("UNKNOWN INSTRUCTION"); 
          return;
 }     
 AC=(Type>>11)&7; 
 M=(Type>>15)&7;
 printf("Z0%ld=YC%ld %s, FC%ld=",P,AC,SHIFTS[S],AC);
 if (MAC1[M]) {
       if (MAC1[M]<0) printf("-");
       printf("DC%ld",AC);
 } 
 printf("%s",MAC2[M]);
 printf("AC%ld*AC%ld",AC,BS);
} 

void MAC0100(long int Code)
{
 long int Type,AC,BS,AS,M,MD;
 Type=Code&0x3FFFF;
 BS=Type&7;
 AS=(Type>>8)&7;
 MD=(Type>>6)&3;
 if (MD) {
          printf("UNKNOWN INSTRUCTION"); 
          return;
 }     
 AC=(Type>>11)&7; 
 M=(Type>>15)&7;
 printf("FC%ld=",AC);
 if (MAC1[M]) {
       if (MAC1[M]<0) printf("-");
       printf("DC%ld",AC);
 } 
 printf("%s",MAC2[M]);
 printf("AC%ld*AC%ld",AS,BS);
} 

void MAC0101(long int Code)
{
 long int Type,AC,BS,S,MD,P,M;
 Type=Code&0x3FFFF;
 P=Type&3;
 BS=(Type>>8)&7;
 S=(Type>>6)&3;
 MD=(Type>>2)&1;
 if (MD) {
          printf("UNKNOWN INSTRUCTION"); 
          return;
 }     
 AC=(Type>>11)&7; 
 M=(Type>>15)&7;
 printf("Z1%ld=YC%ld %s, FC%ld=",P,AC,SHIFTS[S],AC);
 if (MAC1[M]) {
       if (MAC1[M]<0) printf("-");
       printf("DC%ld",AC);
 } 
 printf("%s",MAC2[M]);
 printf("Z0%ld*AC%ld",P,BS);
} 

void MAC0110(long int Code)
{
 long int Type,AC,AS,S,MD,P,M;
 Type=Code&0x3FFFF;
 P=Type&3;
 AS=(Type>>8)&7;
 S=(Type>>6)&3;
 MD=(Type>>2)&1;
 if (MD) {
          printf("UNKNOWN INSTRUCTION"); 
          return;
 }     
 AC=(Type>>11)&7; 
 M=(Type>>15)&7;
 printf("Z0%ld=YC%ld %s, FC%ld=",P,AC,SHIFTS[S],AC);
 if (MAC1[M]) {
       if (MAC1[M]<0) printf("-");
       printf("DC%ld",AC);
 } 
 printf("%s",MAC2[M]);
 printf("AC%ld*Z1%ld",AS,P);
} 

void MAC0111(long int Code)
{
 long int Type,AC,BS,S,MD,P,M;
 Type=Code&0x3FFFF;
 P=Type&3;
 BS=(Type>>8)&7;
 S=(Type>>6)&3;
 MD=(Type>>2)&1;
 if (MD) {
          printf("UNKNOWN INSTRUCTION"); 
          return;
 }     
 AC=(Type>>11)&7; 
 M=(Type>>15)&7;
 printf("Z1%ld=YC%ld %s, FC%ld=",P,AC,SHIFTS[S]);
 if (MAC1[M]) {
       if (MAC1[M]<0) printf("-");
       printf("DC%ld",AC);
 } 
 printf("%s",MAC2[M]);
 printf("AC%ld*AC%ld",AC,BS);
} 

void MAC1xxx(long int Code)
{
 long int Type,AC,I,P[2],S[2],M;
 Type=Code&0x3FFFF;
 P[0]=(Type>>8)&3;S[0]=(Type>>10)&1;
 P[1]=Type&3;S[1]=(Type>>2)&1;
 I=(Type>>3)&31;
 AC=(Type>>11)&7; 
 M=(Type>>15)&7;
 printf("FC%ld=",AC);
 if (MAC1[M]) {
       if (MAC1[M]<0) printf("-");
       printf("DC%ld",AC);
 } 
 printf("%s",MAC2[M]);
 printf("Z0%ld",P[0]);
 if (S[0]) printf("[%ld]",I);
 printf("*Z1%ld",P[1]);
 if (S[1]) printf("[%ld]",I);
} 

void SolveMAC(long int Code)
{
 long int Type;
 Type=((Code>>11)&8)|((Code>>3)&7);
 switch(Type) {
      case 0: MAC0000(Code);
              break;
      case 1: MAC0001(Code);
              break;
      case 2: MAC0010(Code);
              break;
      case 3: MAC0011(Code);
              break;
      case 4: MAC0100(Code);
              break;
      case 5: MAC0101(Code);
              break;
      case 6: MAC0110(Code);
              break;
      case 7: MAC0111(Code);
              break;
      case 8: 
      case 9:
      case 10: 
      case 11:
      case 12:
      case 13:
      case 14:
      case 15: MAC1xxx(Code);
               break;
 }               
} 

int CLI(int argc,char *argv[])
{
 int i,T=0,j;
 for(i=1;i<argc;i++)
  if (argv[i][0]=='-')
   {
    j=1;
    while(argv[i][j]!=0)
    {
     switch (argv[i][j])  {
       case '?': 
       case 'h': T=T|1;
                 break;
       case 'o': T=T|2;
                 break;
       case 'p': T=T|4;
                 break;
       default: T=T|256;            
     }    
     j++;
    }     
   }    
 return(T);
}
 
int main(int argc, char *argv[])
{
  long int Type,Code,i,T;
  if (argc!=0) T=CLI(argc,argv);
  if (T&256) printf("Option error, for usage use -h\n");
  if (T&1) {
            printf("Option:\n");
            printf(" -h: display this help screen\n");
            printf(" -o: display opcode in hex\n");
            printf(" -p: display memory position\n");
       }     
  if (!(T&257)) 
   for(i=0;i<0x435;i++)
   {
    Code=D1_800_435_buffer[i];
    Type=(Code&0xC0000)>>18;
    if (T&4) printf("%04X:",0x800+i);
    if (T&2) printf("(%06X):",Code);
    switch (Type) {
          case 0: SolveJMP(Code); // Ok
                  break;
          case 1: SolveMOV(Code); // Ok
                  break;
          case 2: SolveALU(Code); // Ok
                  break;
          case 3: SolveMAC(Code); // Ok
                  break;
    }                 
    printf("\n");
   }
  return(T>>8);
}
