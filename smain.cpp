#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef int b32;

#define ARRAY_COUNT(array) (sizeof(array)/sizeof((array)[0]))
#define EMPTY_LINE_WIDTH


typedef struct
{
	char *Text;
	int Length;
}string;

enum {
	NOP,
	TFA,
	TFO,
	SWP,
	JMP,
	JE,
	JNE,
	PRT,
	PXL,
	COL,
	CLR
}op_code;

enum {
	MME = -2,
	QP,
	NULL_REGISTER
} operand;


typedef struct
{
	int opcode;
	int src;
	int des;
}instruction;

enum {
	COMPILER_NONE = 0,
	COMPILER_QUEUEING = 1,
	COMPILER_CODE = 2
}compiler_mode;


b32
WhiteSpaceCharacter(char character)
{
	b32 Result = false;
	if (character == 9 || character == 32)
	{
		Result = true;
	}

	return Result;
}

b32
AcceptableCharacter(char character)
{
	b32 Result = false;
	if ((character >= 33 && character < 127) || character == '\0')
	{
		Result = true;
	}

	return Result;
}

b32
NumericalCharacter(char character)
{
	b32 Result = false;
	if (character >= 48 && character <= 57)
	{
		Result = true;
	}
	return Result;
}

b32
LowercaseCharacter(char character)
{
	b32 Result = false;
	if (character >= 97 && character <= 122)
	{
		Result = true;
	}
	return Result;
}

b32
UppercaseCharacter(char character)
{
	b32 Result = false;
	if (character >= 65 && character <= 90)
	{
		Result = true;
	}
	return Result;
}

b32 
AlphabeticalCharacter(char character)
{
	b32 Result = false;
	if (UppercaseCharacter(character) || LowercaseCharacter(character))
	{
		Result = true;
	}
	return Result;
}

b32
AlphaNumericCharacter(char character)
{
	b32 Result = false;
	if (NumericalCharacter(character) || AlphabeticalCharacter(character))
	{
		Result = true;
	}
	return Result;
}

void
ToUpper(string Body)
{
	for (int i = 0; i < Body.Length; ++i)
	{
		if (LowercaseCharacter(Body.Text[i]))
		{
			Body.Text[i] -= 32;
		}
	}
}

void
ToLower(string Body)
{
	for (int i = 0; i < Body.Length; ++i)
	{
		if (UppercaseCharacter(Body.Text[i]))
		{
			Body.Text[i] += 32;
		}
	}
}

int
ToInt(string Body)
{
	int Num = 0;
	int Power = 1;
	for (int l = Body.Length - 1; l >= 0; --l)
	{
		if (Body.Text[l] == '0')
		{
			//NOTE(ian): do nothing
		}
		else if (Body.Text[l] == '1')
		{
			Num += Power;
		}
		else if (Body.Text[l] == '2')
		{
			Num += 2 * Power;
		}
		else if (Body.Text[l] == '3')
		{
			Num += 3 * Power;
		}
		else if (Body.Text[l] == '4')
		{
			Num += 4 * Power;
		}
		else if (Body.Text[l] == '5')
		{
			Num += 5 * Power;
		}
		else if (Body.Text[l] == '6')
		{
			Num += 6 * Power;
		}
		else if (Body.Text[l] == '7')
		{
			Num += 7 * Power;
		}
		else if (Body.Text[l] == '8')
		{
			Num += 8 * Power;
		}
		else if (Body.Text[l] == '9')
		{
			Num += 9 * Power;
		}

		Power *= 10;
	}

	return Num;
}

b32
EqualString(string A, string B)
{
	b32 Result = false;
	if (A.Length == B.Length)
	{
		Result = true;
		for (int i = 0; i < A.Length; ++i)
		{
			if (A.Text[i] != B.Text[i])
			{
				Result = false;
				break;
			}
		}
	}
	return Result;
}

b32
EqualString(string A, const char B[], int BLength)
{
	b32 Result = false;
	if (A.Length == BLength)
	{
		Result = true;
		for (int i = 0; i < A.Length; ++i)
		{
			if (A.Text[i] != B[i])
			{
				Result = false;
				break;
			}
		}
	}
	return Result;
}

int
FindString(const char Text[], int TextLength,const char Pattern[], int PatternLength)
{
	int i = 0;
	while (i <= TextLength - PatternLength + 1)
	{
		b32 Match = true;
		for (int j = 0; j < PatternLength; ++j)
		{
			if (Pattern[j] != Text[i + j])
			{
				Match = false;
				break;
			}
		}
		if (Match)
		{
			break;
		}
		++i;
	}


	if (i == (TextLength - PatternLength + 2))
	{
		i = -1;
	}

	return i;
}

void
RemoveCharacter(string Body, int start, int end)
{
	int diff = end - start;
	for (int i = start; i < Body.Length; ++i)
	{
		Body.Text[i] = Body.Text[i + diff];
	}
}

void
RemoveCharacter(char *String, int Length, int Start, int End)
{
	int Diff = End - Start;
	for (int i = Start; i < Length; ++i)
	{
		String[i] = String[i + Diff];
	}
}



void RemoveSpecialCharacters(string Body)
{
	for (int i = 0; i < Body.Length; ++i)
	{
		if (Body.Text[i] == ';')
		{
			int index = i;
			while (Body.Text[index] != '\n')
			{
				++index;
			}

			RemoveCharacter(Body, i, index);
		}
	}
}

void
RemoveSpecialCharacters(char*String, int Length)
{
	for (int i = 0; i < Length; ++i)
	{
		if (String[i] < 0 || String[i] > 127)
		{
			int index = i;
			while (String[index] < 0 || String[index] > 127)
			{
				++index;
			}

			RemoveCharacter(String,Length, i, index);
		}
	}
}


int
ParseOperand(string Operand)
{
	string Copy;
	Copy.Length = Operand.Length;
	Copy.Text = (char*)malloc((Copy.Length + 1) * sizeof(char));
	Copy.Text[Copy.Length] = 0;
	for (int i = 0; i < Copy.Length; ++i)
	{
		Copy.Text[i] = Operand.Text[i];
	}
	ToLower(Copy);

	int Result = NULL_REGISTER;
	if (Copy.Text[0] == 'r')
	{
		string Number;
		Number.Text = &Copy.Text[1];
		Number.Length = 0;
		for (int i = 1; i < Copy.Length; ++i)
		{
			if (NumericalCharacter(Copy.Text[i]))
			{
				++Number.Length;
			}
			else
			{
				break;
			}
		}

		if (Number.Length > 0)
		{
			Result = ToInt(Number);
		}
	}
	else if (EqualString(Copy, "qp", 2))
	{
		Result = QP;
	}
	else if (EqualString(Copy, "mme", 3))
	{
		Result = MME;
	}
	free(Copy.Text);
	return Result;
}


void ReadLine(FILE * File, char *Buffer, int Size)
{
	memset((void*)Buffer, 0, Size);

	char C = fgetc(File);

	int Count = 0;
	while (C != '\n' && C != '\0' && C != EOF && Count < Size)
	{

		Buffer[Count] = C;

		C = fgetc(File);

		++Count;
	}
}




void
PrintCharString(char *string)
{
	printf("\n");
	for (int i = 0; i < strlen(string); ++i)
	{
		printf("%cb", string[i]);
	}
	printf("\n");
}

void Disassemble(char *Line)
{
	int length = strlen(Line);


	if (length > 0)

	{
		//NOTE(ian): Delete whitespace characters
		for (int i = 0; Line[i] != '\0'; ++i)
		{
			while (!AcceptableCharacter(Line[i]))
			{
				for (int j = i; Line[j] != '\0'; ++j)
				{
					Line[j] = Line[j + 1];
				}
			}
		}


		b32 Expression = false;
		char * Second = 0;
		char temp;
		int index;
		for (int i = 0; i < length - 1; ++i)
		{
			if (Line[i] == '-' && Line[i + 1] == '>')
			{
				Expression = true;
				index = i;
				temp = Line[i];
				Line[i] = '\0';
				Second = Line + (i + 2);
				break;
			}
		}

		if (Expression)
		{

			printf("REPLACE '%s', '%s'\n", Line, Second);
			Line[index] = temp;
		}
		else
		{
			printf("LOADINPUT '%s'\n", Line);
		}

	}
}


int
main(int argc, char* argv[])
{
	/*
	printf("Total arguments = %d", argc);
	for (int i = 0; i < argc; ++i)
	{
		printf("\nArgument #%d: %s", i,argv[i]);
	}
	*/

	//int y;
	//scanf_s("%d", &y);


	FILE *f;
	fopen_s(&f,"stest.txt", "r");

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	string ProgramBody;
	ProgramBody.Length = fsize + 2;
	ProgramBody.Text = (char*)malloc(ProgramBody.Length);
	fread((void*)ProgramBody.Text, fsize, 1, f);
	fclose(f);

	ProgramBody.Text[fsize] = 10;
	ProgramBody.Text[fsize + 1] = 0;

	printf("%s", ProgramBody.Text);


	RemoveSpecialCharacters(ProgramBody);

	printf("\n\n%s",ProgramBody.Text);

	//NOTE(ian): split into lines
	int NumLines = 0;
	for (int i = 0; i < ProgramBody.Length; ++i)
	{
		if (ProgramBody.Text[i] == '\n')
		{
			++NumLines;
		}
	}
	int* LineLengths = (int*)malloc((NumLines+1)*sizeof(int));
	
	LineLengths[0] = 0;
	int LineWidth = 0;
	int LineIndex = 1;
	for (int i = 0; i < ProgramBody.Length; ++i)
	{
		//NOTE(ian): Optimize?
		if (ProgramBody.Text[i] != '\n')
		{
			++LineWidth;
		}
		else
		{
			LineLengths[LineIndex++] = ++LineWidth;
		}
	}

	int NumRegisters = 10;
	int *Registers = (int*)malloc(NumRegisters * sizeof(int));
	int CompilerMode = COMPILER_NONE;
	int MaxInstructions = 5000;
	instruction* ByteCode = (instruction*)malloc(MaxInstructions * sizeof(instruction));
	int InstructionCount = 0;
	int QueueMaxSize = 1000;
	int *Queue = (int *)malloc(QueueMaxSize * sizeof(int));
	int QueueSize = 0;
	int MainMemory = 1000000;
	int ErrorCount = 0;
	for (int i = 0; i < NumLines; ++i)
	{
		if (CompilerMode == COMPILER_NONE)
		{
			if (FindString(&ProgramBody.Text[LineLengths[i]], LineLengths[i + 1] - LineLengths[i] - 1, ".", 1) != -1)
			{
				if (FindString(&ProgramBody.Text[LineLengths[i]], LineLengths[i + 1] - LineLengths[i] - 1, ".data", 5) != -1)
				{
					CompilerMode = COMPILER_QUEUEING;
				}
				else if (FindString(&ProgramBody.Text[LineLengths[i]], LineLengths[i + 1] - LineLengths[i] - 1, ".code", 5) != -1)
				{
					CompilerMode = COMPILER_CODE;
				}
				else
				{
					CompilerMode = COMPILER_NONE;
				}
			}
		}
		else if (CompilerMode == COMPILER_QUEUEING)
		{
			b32 SwitchMode = false;
			for (int j = LineLengths[i]; j < LineLengths[i + 1]; ++j)
			{
				if (ProgramBody.Text[j] == '.')
				{
					SwitchMode = true;
					--i;
					CompilerMode = COMPILER_NONE;
				}
			}

			if (!SwitchMode && (LineLengths[i+1] - LineLengths[i] - 1) > 0)
			{
				for (int j = LineLengths[i]; j < LineLengths[i + 1] - 1; ++j)
				{
					if (NumericalCharacter(ProgramBody.Text[j]))
					{
						int k = j;
						while (NumericalCharacter(ProgramBody.Text[k]) && k < LineLengths[i + 1] - 1)
						{
							++k;
						}

						int Num = ToInt(string{ &ProgramBody.Text[j],k - j });

						if (QueueSize + 1 > QueueMaxSize)
						{
							printf("\nERROR - Line %d: Max Queue Size Reached!!", i + 1);
							++ErrorCount;
							CompilerMode = COMPILER_NONE;
							break;
						}

						if (MainMemory - Num < 0)
						{
							printf("\nERROR - Line %d: Main Memory is empty. There is nothing more to take.", i + 1);
							++ErrorCount;
							CompilerMode = COMPILER_NONE;
							break;
						}
						Queue[QueueSize++] = Num;
						MainMemory -= Num;

						j += k - j - 1;
					}
				}
			}
		}
		else if (CompilerMode == COMPILER_CODE)
		{
			b32 SwitchMode = false;
			for (int j = LineLengths[i]; j < LineLengths[i + 1]; ++j)
			{
				if (ProgramBody.Text[j] == '.')
				{
					SwitchMode = true;
					--i;
					CompilerMode = COMPILER_NONE;
				}
			}


			if (!SwitchMode && (LineLengths[i+1] - LineLengths[i]-1) > 0)
			{
				b32 ScanningInstruction = false;
				string instr = {};
				int EndOpcodeIndex = 0;
				for (int j = LineLengths[i]; j < LineLengths[i + 1]; ++j)
				{
					if (!ScanningInstruction)
					{
						if (AlphabeticalCharacter(ProgramBody.Text[j]))
						{
							instr.Text = &ProgramBody.Text[j];
							instr.Length = 1;
							ScanningInstruction = true;
						}
					}
					else
					{
						if (AlphabeticalCharacter(ProgramBody.Text[j]))
						{
							++instr.Length;
						}
						else
						{
							EndOpcodeIndex = j;
							break;
						}
					}
				}

				string Copy = {};
				Copy.Length = instr.Length;
				Copy.Text = (char*)malloc((Copy.Length+1) * sizeof(char));
				for (int j = 0; j < Copy.Length; ++j)
				{
					Copy.Text[j] = instr.Text[j];
				}
				Copy.Text[Copy.Length] = 0;

				ToLower(Copy);


				/*
				NOP,
				TFA,
				TFO,
				SWP,
				JMP,
				JE,
				JNE,
				PRT,
				PXL,
				COL,
				CLR
				*/

				instruction Potential;


				if (EqualString(Copy, "nop", 3))
				{
					Potential.opcode = NOP;
				}
				else if (EqualString(Copy, "tfa", 3))
				{
					Potential.opcode = TFA;
				}
				else if (EqualString(Copy, "tfo", 3))
				{
					Potential.opcode = TFO;
				}
				else if (EqualString(Copy, "swp", 3))
				{
					Potential.opcode = SWP;
				}
				else if (EqualString(Copy, "jmp", 3))
				{
					Potential.opcode = JMP;
				}
				else if (EqualString(Copy, "je", 3))
				{
					Potential.opcode = JE;
				}
				else if (EqualString(Copy, "jne", 3))
				{
					Potential.opcode = JNE;
				}
				else if (EqualString(Copy, "prt", 3))
				{
					Potential.opcode = PRT;
				}
				else if (EqualString(Copy, "pxl", 3))
				{
					Potential.opcode = PXL;
				}
				else if (EqualString(Copy, "col", 3))
				{
					Potential.opcode = COL;
				}
				else if (EqualString(Copy, "clr", 3))
				{
					Potential.opcode = CLR;
				}
				else
				{
					Potential.opcode = -1;
					ToUpper(Copy);
					++ErrorCount;
					printf("\nERROR - Line %d: %s is not a known instruction!", i+1, Copy.Text);
				}

				if (Potential.opcode == NOP || Potential.opcode == CLR)
				{
					ByteCode[InstructionCount++] = Potential;
				}
				else if (Potential.opcode == JMP || Potential.opcode == JE || Potential.opcode == JNE || Potential.opcode == COL)
				{

				}
				else if(Potential.opcode != -1)
				{
					string OperandOne = {};
					string OperandTwo = {};
					int ScanningOperands = 0;
					for (int j = EndOpcodeIndex; j < LineLengths[i + 1]; ++j)
					{
						if (ScanningOperands == 0)
						{
							if (AlphaNumericCharacter(ProgramBody.Text[j]))
							{
								OperandOne.Text = &ProgramBody.Text[j];
								OperandOne.Length = 1;
								ScanningOperands = 1;
							}
						}
						else if (ScanningOperands == 1)
						{
							if (AlphaNumericCharacter(ProgramBody.Text[j]))
							{
								++OperandOne.Length;
							}
							else
							{
								ScanningOperands = 2;
							}
						}
						else if (ScanningOperands == 2)
						{
							if (AlphaNumericCharacter(ProgramBody.Text[j]))
							{
								OperandTwo.Text = &ProgramBody.Text[j];
								OperandTwo.Length = 1;
								ScanningOperands = 3;
							}
						}
						else if (ScanningOperands == 3)
						{
							if (AlphaNumericCharacter(ProgramBody.Text[j]))
							{
								++OperandTwo.Length;
							}
							else
							{
								ScanningOperands = 4;
								break;
							}
						}

					}

					if (ScanningOperands < 4)
					{
						ToUpper(Copy);
						printf("\nERROR - Line %d: %s requires 2 operands", i+1,Copy.Text);
						++ErrorCount;
					}
					else
					{
						Potential.src = NULL_REGISTER;
						Potential.des = NULL_REGISTER;

						int OpOne = ParseOperand(OperandOne);
						if ((OpOne > 0 && OpOne <= NumRegisters) || OpOne == QP || OpOne == MME)
						{
							Potential.src = OpOne;
						}
						else
						{
							string Temp;
							Temp.Length = OperandOne.Length;
							Temp.Text = (char*)malloc((Temp.Length + 1) * sizeof(char));
							for (int j = 0; j < Temp.Length; ++j)
							{
								Temp.Text[j] = OperandOne.Text[j];
							}
							Temp.Text[Temp.Length] = 0;
							ToUpper(Temp);

							if (OpOne > NumRegisters)
							{
								printf("\nERROR - Line %d: %s is not a valid register because there are only %d registers.", i, Temp.Text,NumRegisters);
							}
							else
							{
								printf("\nERROR - Line %d: %s is not a valid operand", i + 1, Temp.Text);
							}
							++ErrorCount;
							free(Temp.Text);
						}

						int OpTwo = ParseOperand(OperandOne);
						if ((OpTwo > 0 && OpTwo <= NumRegisters) || OpTwo == QP || OpTwo == MME)
						{
							Potential.des = OpOne;
						}
						else
						{
							string Temp;
							Temp.Length = OperandTwo.Length;
							Temp.Text = (char*)malloc((Temp.Length + 1) * sizeof(char));
							for (int j = 0; j < Temp.Length; ++j)
							{
								Temp.Text[j] = OperandTwo.Text[j];
							}
							Temp.Text[Temp.Length] = 0;
							ToUpper(Temp);

							if (OpTwo > NumRegisters)
							{
								printf("\nERROR - Line %d: %s is not a valid register because there are only %d registers.", i, Temp.Text, NumRegisters);
							}
							else
							{
								printf("\nERROR - Line %d: %s is not a valid operand", i + 1, Temp.Text);
							}
							++ErrorCount;
							free(Temp.Text);
						}

						b32 ProperInstruction = (Potential.src != NULL_REGISTER && Potential.des != NULL_REGISTER);

						if (Potential.opcode == TFA)
						{

						}
						else if (Potential.opcode == TFO)
						{
							if (Potential.src == QP)
							{
								ProperInstruction = false;
								printf("\nERROR - Line %d: Queue Pointer cannot be the first operand in a TFO instruction", i+1);
								//TODO(ian): Come up with a better error message
								++ErrorCount;
							}
						}
						else if (Potential.opcode == SWP)
						{
							if (Potential.src == QP)
							{
								ProperInstruction = false;
								printf("\nERROR - Line %d: Queue Pointer cannot be the first operand in a SWP instruction", i+1);
								++ErrorCount;
							}
							if (Potential.des == QP)
							{
								ProperInstruction = false;
								printf("\nERROR - Line %d: Queue Pointer cannot be the second operand in a SWP instruction", i+1);
								++ErrorCount;
							}
							
						}
						else if (Potential.opcode == PRT)
						{
							if (Potential.src == QP)
							{
								ProperInstruction = false;
								printf("\nERROR - Line %d: Queue Pointer cannot be the first operand in a PRT instruction", i+1);
								++ErrorCount;
							}
							if (Potential.des == QP)
							{
								ProperInstruction = false;
								printf("\nERROR - Line %d: Queue Pointer cannot be the second operand in a PRT instruction", i+1);
								++ErrorCount;
							}
						}
						else if (Potential.opcode == PXL)
						{
							if (Potential.src == QP)
							{
								ProperInstruction = false;
								printf("\nERROR - Line %d: Queue Pointer cannot be the first operand in a PXL instruction", i+1);
								++ErrorCount;
							}
							if (Potential.des == QP)
							{
								ProperInstruction = false;
								printf("\nERROR - Line %d: Queue Pointer cannot be the second operand in a PXL instruction", i+1);
								++ErrorCount;
							}
						}

						if (ProperInstruction)
						{
							ByteCode[InstructionCount++] = Potential;
						}
					}
				}
				free(Copy.Text);
			}

			
			
		}


	}

	

	if (ErrorCount > 0)
	{
		printf("\nCOMPILATION FAILED - %d errors", ErrorCount);
	}
	else
	{

	}


	int x;


	scanf_s("%d", &x);












	





	return 0;
}










