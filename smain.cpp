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
	NONHEADER,
	DATA,
	SIM,
	CODE,
	END
}section_header;


typedef struct
{
	int Start;
	int End;
	int Type;
}section_marker;

enum {
	NULL_INSTRUCTION,
	NOP,
	JMP,
	JE,
	JNE,
	TFA,
	TFO,
	SWP,
	PRT,
	PXL,
	COL,
	DRW,
	SND,
	PLY,
	UD,
	LR,
	AB,
	EE,
	BRK
}op_code;

enum {
	SIM_REGISTER = -3,
	MME,
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
	COMPILER_CODE = 2,
	COMPILER_SIM = 4,
	COMPILER_NO_SIM = 8
}compiler_mode;


typedef struct {
	string Name;
	int InstructionLine;
	int LineNum;
}label;

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
PrintString(string Body)
{
	for (int i = 0; i < Body.Length; ++i)
	{
		printf("%c", Body.Text[i]);
	}
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
FindString(const char Text[], int TextLength, const char Pattern[], int PatternLength)
{
	if (TextLength != 0 && PatternLength != 0)
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
	return -1;
}

int
FindString(char *Text, int TextLength, char *Pattern, int PatternLength)
{
	if (TextLength != 0 && PatternLength != 0)
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
	return -1;
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



string
RemoveSpecialCharacters(string Body)
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

			Body.Length -= index - i;
		}
		else if (Body.Text[i] > 127 || Body.Text[i] < 0)
		{
			int index = i;
			while (Body.Text[index] > 127 || Body.Text[index] < 0)
			{
				++index;
			}


			RemoveCharacter(Body, i, index);

			Body.Length -= index - i;
		}
	}
	return Body;
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

			RemoveCharacter(String, Length, i, index);
		}
	}
}

typedef struct {
	string String;
	b32 Found;
}found_string;


found_string
ParseLabel(string Line)
{
	int LabelMode = 0;
	found_string Result = { string{ 0,0 },false };
	for (int j = 0; j < Line.Length; ++j)
	{
		if (LabelMode == 0)
		{
			if (AlphaNumericCharacter(Line.Text[j]))
			{
				LabelMode = 1;
				Result.String.Text = &Line.Text[j];
				Result.String.Length = 1;
			}
		}
		else if (LabelMode == 1)
		{
			if (Line.Text[j] == ':')
			{
				Result.Found = true;
				break;
			}
			else if (AlphaNumericCharacter(Line.Text[j]))
			{
				++Result.String.Length;
			}
			else
			{
				Result.Found = false;
				break;
			}
		}
	}

	return Result;
}

int
ParseParameters(string Line, string *Parameters, int ParameterBufferLength)
{
	int NumParameters = 0;
	int ParameterScan = 0;
	for (int j = 0; j < Line.Length; ++j)
	{
		if ((ParameterScan & 1) == 0)
		{
			if (AlphaNumericCharacter(Line.Text[j]))
			{
				if (NumParameters < ParameterBufferLength)
				{
					++ParameterScan;
					Parameters[NumParameters].Text = &Line.Text[j];
					Parameters[NumParameters].Length = 1;
				}
				else
				{
					++(NumParameters);
					break;
				}
			}
		}
		else
		{
			if (AlphaNumericCharacter(Line.Text[j]))
			{
				++Parameters[NumParameters].Length;
			}
			else
			{
				++ParameterScan;
				++(NumParameters);
			}

		}
	}
	return NumParameters;
}

found_string
ParseInstruction(string Line)
{
	b32 ScanningInstruction = false;
	string instr = { 0 };
	int EndOpcodeIndex = 0;
	for (int j = 0; j < Line.Length; ++j)
	{
		if (!ScanningInstruction)
		{
			if (AlphabeticalCharacter(Line.Text[j]))
			{
				instr.Text = &Line.Text[j];
				instr.Length = 1;
				ScanningInstruction = true;
			}
		}
		else
		{
			if (AlphabeticalCharacter(Line.Text[j]))
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
	found_string Result = { instr,-1 };
	if (instr.Length > 0)
	{
		if (EqualString(instr, "NOP", 3))
		{
			Result.Found = NOP;
		}
		else if (EqualString(instr, "TFA", 3))
		{
			Result.Found = TFA;
		}
		else if (EqualString(instr, "TFO", 3))
		{
			Result.Found = TFO;
		}
		else if (EqualString(instr, "SWP", 3))
		{
			Result.Found = SWP;
		}
		else if (EqualString(instr, "JMP", 3))
		{
			Result.Found = JMP;
		}
		else if (EqualString(instr, "JE", 2))
		{
			Result.Found = JE;
		}
		else if (EqualString(instr, "JNE", 3))
		{
			Result.Found = JNE;
		}
		else if (EqualString(instr, "PRT", 3))
		{
			Result.Found = PRT;
		}
		else if (EqualString(instr, "PXL", 3))
		{
			Result.Found = PXL;
		}
		else if (EqualString(instr, "COL", 3))
		{
			Result.Found = COL;
		}
		else if (EqualString(instr, "BRK", 3))
		{
			Result.Found = BRK;
		}
	}

	return Result;
}

//NOTE(ian): Assumes string is uppercase
int
DetermineOperand(string Operand)
{
	int Result = NULL_REGISTER;
	if (Operand.Text[0] == 'R')
	{
		if (Operand.Length > 1)
		{
			string Number;
			Number.Text = &Operand.Text[1];
			Number.Length = 0;
			for (int i = 1; i < Operand.Length; ++i)
			{
				if (NumericalCharacter(Operand.Text[i]))
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
	}
	else if (EqualString(Operand, "QP", 2))
	{
		Result = QP;
	}
	else if (EqualString(Operand, "MME", 3))
	{
		Result = MME;
	}
	return Result;
}
b32
EmptyLine(string Line)
{
	b32 EmptyLine = true;
	for (int j = 0; j < Line.Length; ++j)
	{
		if (AlphaNumericCharacter(Line.Text[j]))
		{
			EmptyLine = false;
			break;
		}
	}
	return EmptyLine;
}

enum {
	AVAILABLE_LABEL = -1,
	TAKEN
}label_state;


int
TakenLabel(string Label, label *Labels, int MaxLabels, int NumLabels)
{
	int TakenLabel = AVAILABLE_LABEL;
	for (int j = 0; j < NumLabels; ++j)
	{

		if (EqualString(Label, Labels[j].Name))
		{
			TakenLabel = j;
			break;
		}
	}

	return TakenLabel;
}

found_string
HasNewSimInstruction(string Line)
{
	int NewSimIndex = -1;
	for (int j = 0; j < Line.Length; ++j)
	{
		if (Line.Text[j] == '#')
		{
			NewSimIndex = j + 1;
			break;
		}
	}

	string SimName = { 0 };
	int ScanningMode = 0;
	int ParameterIndex = NewSimIndex;
	found_string Result;
	Result.String = SimName;
	Result.Found = -1;
	if (NewSimIndex != -1)
	{
		for (int j = NewSimIndex; j < Line.Length; ++j)
		{
			if (ScanningMode == 0)
			{
				if (AlphaNumericCharacter(Line.Text[j]))
				{
					ScanningMode = 1;
					SimName.Length = 1;
					SimName.Text = &Line.Text[j];
				}
			}
			else if (ScanningMode == 1)
			{
				if (AlphaNumericCharacter(Line.Text[j]))
				{
					++SimName.Length;
				}
				else
				{
					ParameterIndex = j;
					break;
				}
			}
		}
		Result.String = SimName;
		Result.Found = ParameterIndex;
	}



	return Result;
}

int
IsSimInstruction(string Operator, string *SimNames, int NumSimInstructions)
{
	int SimIndex = -1;
	for (int j = 0; j < NumSimInstructions; ++j)
	{
		if (EqualString(Operator, SimNames[j]))
		{
			SimIndex = j;
			break;
		}
	}

	return SimIndex;
}

b32
IsProperInstruction(instruction Potential, int *ErrorCount, int LineNum)
{
	b32 ProperInstruction = true;
	if (Potential.opcode == TFA)
	{

	}
	else if (Potential.opcode == TFO)
	{
		if (Potential.src == QP)
		{
			ProperInstruction = false;
			printf("\nERROR - Line %d: Queue Pointer cannot be the first operand in a TFO instruction", LineNum + 1);
			//TODO(ian): Come up with a better error message
			++(*ErrorCount);
		}
	}
	else if (Potential.opcode == SWP)
	{
		if (Potential.src == QP || Potential.des == QP)
		{
			ProperInstruction = false;
			//TODO(ian): Maybe allow this?
			printf("\nERROR - Line %d: Cannot swap with the Queue Pointer", LineNum + 1);
			++(*ErrorCount);
		}

		if (Potential.src == MME || Potential.des == MME)
		{
			ProperInstruction = false;
			printf("\nERROR - Line %d: Cannot swap with Main Memory", LineNum + 1);
			++(*ErrorCount);
		}
	}
	else if (Potential.opcode == PRT || Potential.opcode == PXL || Potential.opcode == COL)
	{
		if (Potential.src == QP || Potential.des == QP)
		{
			ProperInstruction = false;
			printf("\nERROR - Line %d: Queue Pointer cannot be used for a system instruction", LineNum + 1);
			++(*ErrorCount);
		}

		if (Potential.src == MME || Potential.des == MME)
		{
			ProperInstruction = false;
			printf("\nERROR - Line %d: Main Memory cannot be used for a system instruction", LineNum + 1);
			++(*ErrorCount);
		}
	}

	return ProperInstruction;
}

int
NumInstructionParameters(int Opcode)
{
	/*
	enum {
	NULL_INSTRUCTION,
	NOP,
	JMP,
	JE,
	JNE,
	TFA,
	TFO,
	SWP,
	PRT,
	PXL,
	COL,
	DRW,
	SND,
	PLY,
	UD,
	LR,
	AB,
	EE
	}op_code;
	*/

	int Result = -1;
	if (Opcode == NULL_INSTRUCTION)
	{

	}
	else if (Opcode == NOP || Opcode == BRK)
	{
		Result = 0;
	}
	else if (Opcode == JMP || Opcode == JE || Opcode == JNE)
	{
		Result = 1;
	}
	else if (Opcode == TFA || Opcode == TFO || Opcode == SWP || Opcode == PRT || Opcode == PXL || Opcode == COL)
	{
		Result = 2;
	}

	return Result;
}


void
AddInstruction(instruction Potential, instruction *ByteCode, int *InstructionCount, int *InstructionLines, int Line)
{
	InstructionLines[*InstructionCount] = Line;
	ByteCode[(*InstructionCount)++] = Potential;
}

void
AddLabel(label *Labels, string Label, int *NumLabels, int InstructionCount, int LineNum)
{
	Labels[*NumLabels].InstructionLine = InstructionCount;
	Labels[*NumLabels].LineNum = LineNum;
	Labels[(*NumLabels)++].Name = Label;
}

#define NUM_REGISTERS 25
#define MAX_INSTRUCTIONS 5000
#define MAX_LABELS 1000
#define MAX_MAIN_MEMORY 1000000
#define MAX_QUEUE_SIZE 5000
#define MAX_PARAMETERS 10


void
PrintByteCode(instruction *ByteCode,int ProgramStart, int ProgramEnd)
{
	for (int i = ProgramStart; i < ProgramEnd; ++i)
	{

		printf("\n#%d: ", i);
		if (ByteCode[i].opcode == NOP)
		{
			printf("NOP");
		}
		else if (ByteCode[i].opcode == TFA)
		{
			printf("TFA ");
			if (ByteCode[i].src == MME)
			{
				printf("MME ");
			}
			else if (ByteCode[i].src == QP)
			{
				printf("QP ");
			}
			else if (ByteCode[i].src > NULL_REGISTER)
			{
				printf("R%d ", ByteCode[i].src);
			}

			printf(", ");
			if (ByteCode[i].des == MME)
			{
				printf("MME ");
			}
			else if (ByteCode[i].des == QP)
			{
				printf("QP ");
			}
			else if (ByteCode[i].des > NULL_REGISTER)
			{
				printf("R%d ", ByteCode[i].des);
			}
		}
		else if (ByteCode[i].opcode == TFO)
		{
			printf("TFO ");
			if (ByteCode[i].src == MME)
			{
				printf("MME ");
			}
			else if (ByteCode[i].src == QP)
			{
				printf("QP ");
			}
			else if (ByteCode[i].src > NULL_REGISTER)
			{
				printf("R%d ", ByteCode[i].src);
			}

			printf(", ");
			if (ByteCode[i].des == MME)
			{
				printf("MME ");
			}
			else if (ByteCode[i].des == QP)
			{
				printf("QP ");
			}
			else if (ByteCode[i].des > NULL_REGISTER)
			{
				printf("R%d ", ByteCode[i].des);
			}
		}
		else if (ByteCode[i].opcode == SWP)
		{
			printf("SWP ");
			if (ByteCode[i].src == MME)
			{
				printf("MME ");
			}
			else if (ByteCode[i].src == QP)
			{
				printf("QP ");
			}
			else if (ByteCode[i].src > NULL_REGISTER)
			{
				printf("R%d ", ByteCode[i].src);
			}

			printf(", ");
			if (ByteCode[i].des == MME)
			{
				printf("MME ");
			}
			else if (ByteCode[i].des == QP)
			{
				printf("QP ");
			}
			else if (ByteCode[i].des > NULL_REGISTER)
			{
				printf("R%d ", ByteCode[i].des);
			}
		}
		else if (ByteCode[i].opcode == JMP)
		{
			printf("JMP %d, %d", ByteCode[i].src, ByteCode[i].des);
		}
		else if (ByteCode[i].opcode == JE)
		{
			printf("JE %d, %d", ByteCode[i].src, ByteCode[i].des);
		}

		else if (ByteCode[i].opcode == JNE)
		{
			printf("JNE %d, %d", ByteCode[i].src, ByteCode[i].des);
		}
		else if (ByteCode[i].opcode == PRT)
		{
			printf("PRT ");
			if (ByteCode[i].src == MME)
			{
				printf("MME ");
			}
			else if (ByteCode[i].src == QP)
			{
				printf("QP ");
			}
			else if (ByteCode[i].src > NULL_REGISTER)
			{
				printf("R%d ", ByteCode[i].src);
			}

			printf(", ");
			if (ByteCode[i].des == MME)
			{
				printf("MME ");
			}
			else if (ByteCode[i].des == QP)
			{
				printf("QP ");
			}
			else if (ByteCode[i].des > NULL_REGISTER)
			{
				printf("R%d ", ByteCode[i].des);
			}
		}
		else if (ByteCode[i].opcode == PXL)
		{

		}
		else if (ByteCode[i].opcode == COL)
		{

		}
		else if (ByteCode[i].opcode == BRK)
		{
			printf("BRK");
		}

	}
}

void
AssembleLine(string ProgramBody, instruction *ByteCode, int *InstructionCount, int *InstructionLines, int i, int *LineLengths, int *ErrorCount, label *Labels, int *NumLabels,
	string *SimNames, instruction *SimInstructions, int *NumSimInstructions, int *SimInstructionLines,string *Parameters, int NumParameters, int *SimArgs)
{
	found_string Label = ParseLabel(string{ &ProgramBody.Text[LineLengths[i]],LineLengths[i + 1] - LineLengths[i] - 1 });

	int StartOpcodeIndex = LineLengths[i];

	if (Label.Found)
	{
		StartOpcodeIndex = LineLengths[i] + Label.String.Length + 1;
		int Taken = TakenLabel(Label.String, Labels, MAX_LABELS, *NumLabels);
		if (Taken != AVAILABLE_LABEL)
		{

			char Temp = Label.String.Text[Label.String.Length];
			Label.String.Text[Label.String.Length] = 0;

			printf("\nERROR - Line %d: Label %s already exists on Line %d", i+1, Label.String.Text, Labels[Taken].LineNum);
			++(*ErrorCount);

			Label.String.Text[Label.String.Length] = Temp;

		}
		else
		{
			if (*NumLabels + 1 <= MAX_LABELS)
			{
				//NOTE(ian): numlabels is a pointer!!!
				AddLabel(Labels, Label.String, NumLabels, *InstructionCount, i);
				if (EmptyLine(string{ &ProgramBody.Text[StartOpcodeIndex],LineLengths[i + 1] - StartOpcodeIndex }))
				{
					if (*InstructionCount + 1 <= MAX_INSTRUCTIONS)
					{
						//NOTE(ian): Instruction Count is a pointer
						AddInstruction(instruction{ NOP,0,0 }, ByteCode, InstructionCount, InstructionLines, i);
					}
				}
			}
		}
	}



	if ((LineLengths[i + 1] - StartOpcodeIndex - 1) > 0)
	{
		string Line = { &ProgramBody.Text[StartOpcodeIndex],LineLengths[i + 1] - StartOpcodeIndex - 1 };

		instruction Potential = { 0,0,0 };

		found_string ParsedInstr = ParseInstruction(Line);
		Potential.opcode = ParsedInstr.Found;


		string Operands[MAX_PARAMETERS];

		int NumOperands = ParseParameters({ &ProgramBody.Text[StartOpcodeIndex + ParsedInstr.String.Length + 1],LineLengths[i + 1] - StartOpcodeIndex - ParsedInstr.String.Length}, Operands, MAX_PARAMETERS);
		int SimIndex = IsSimInstruction(ParsedInstr.String, SimNames, *NumSimInstructions);

		int RequiredArgs = -1;
		if (Potential.opcode != -1 || SimIndex != -1)
		{
			if (Potential.opcode != -1 && SimIndex != -1)
			{
				//TODO(ian): Do something?
			}
			else if (Potential.opcode != -1)
			{
				RequiredArgs = NumInstructionParameters(Potential.opcode);
			}
			else if (SimIndex != -1)
			{
				RequiredArgs = SimArgs[SimIndex];
			}
		}

		if (Potential.opcode == -1 && SimIndex == -1)
		{
			char Temp = ParsedInstr.String.Text[ParsedInstr.String.Length];
			ParsedInstr.String.Text[ParsedInstr.String.Length] = 0;
			++(*ErrorCount);
			printf("\nERROR - Line %d: %s is either an unknown instruction or an improper label", i + 1, ParsedInstr.String.Text);

			ParsedInstr.String.Text[ParsedInstr.String.Length] = Temp;
		}
		else if(NumOperands != RequiredArgs)
		{
			char Temp = ParsedInstr.String.Text[ParsedInstr.String.Length];
			ParsedInstr.String.Text[ParsedInstr.String.Length] = 0;
			++(*ErrorCount);
			printf("\nERROR - Line %d: %s requires exactly %d operands", i + 1, ParsedInstr.String.Text, RequiredArgs);

			ParsedInstr.String.Text[ParsedInstr.String.Length] = Temp;
		}

		b32 CorrectOperands = true;
		int OperandCodes[MAX_PARAMETERS];
		for (int j = 0; j < NumOperands; ++j)
		{
			//TODO(ian): change this to get rid of OpCode!!!
			int OpCode = DetermineOperand(Operands[j]);

			for (int k = 0; k < NumParameters; ++k)
			{
				if (EqualString(Operands[j], Parameters[k]))
				{
					OpCode = (k * -1) + SIM_REGISTER;
					break;
				}
			}
			OperandCodes[j] = OpCode;
			if ((OpCode > 0 && OpCode <= NUM_REGISTERS) || OpCode == QP || OpCode == MME || OpCode <= SIM_REGISTER || (Potential.opcode == JE || Potential.opcode == JNE || Potential.opcode == JMP))
			{
			}
			else
			{
				CorrectOperands = false;

				char Temp = Operands[j].Text[Operands[j].Length];
				Operands[j].Text[Operands[j].Length] = 0;


				if (OpCode > NUM_REGISTERS)
				{
					printf("\nERROR - Line %d: %s is not a valid register because there are only %d registers.", i + 1, Operands[j].Text, NUM_REGISTERS);
				}
				else
				{
					printf("\nERROR - Line %d: %s is not a valid operand/parameter", i + 1, Operands[j].Text);
				}
				Operands[j].Text[Operands[j].Length] = Temp;
				++(*ErrorCount);
			}
		}

		if (CorrectOperands && NumOperands == RequiredArgs && RequiredArgs != -1)
		{
			if (Potential.opcode != -1)
			{
				if (!(Potential.opcode == JNE || Potential.opcode == JE || Potential.opcode == JMP))
				{
					Potential.src = OperandCodes[0];
					Potential.des = OperandCodes[1];
				}

				//NOTE(ian): Error Count is pointer
				b32 ProperInstruction = IsProperInstruction(Potential, ErrorCount, i);
				if (ProperInstruction)
				{
					InstructionLines[*InstructionCount] = i;
					ByteCode[(*InstructionCount)++] = Potential;
				}
			}
			else if (SimIndex != -1)
			{
				int FirstInstrIndex = 0;
				for (int j = 0; j < SimIndex; ++j)
				{
					FirstInstrIndex += SimInstructions[FirstInstrIndex].opcode + 1;
				}

				for (int j = 0; j < SimInstructions[FirstInstrIndex].opcode; ++j)
				{
					instruction Potential = { 0,0,0 };
					int Index = j + FirstInstrIndex + 1;
					Potential.opcode = SimInstructions[Index].opcode;
					int OpcodeParametersNum = NumInstructionParameters(Potential.opcode);
					if (OpcodeParametersNum > 0)
					{
						if (Potential.opcode == JMP || Potential.opcode == JE || Potential.opcode == JNE)
						{
							Potential.src = SimInstructions[Index].src;
							Potential.des = SimInstructions[Index].des;
						}
						else
						{
							if (SimInstructions[Index].src <= SIM_REGISTER)
							{
								Potential.src = OperandCodes[(SimInstructions[Index].src * -1) + SIM_REGISTER];
							}
							else
							{
								Potential.src = SimInstructions[Index].src;
							}

							if (OpcodeParametersNum > 1)
							{
								if (SimInstructions[Index].des <= SIM_REGISTER)
								{
									Potential.des = OperandCodes[(SimInstructions[Index].des * -1) + SIM_REGISTER];
								}
								else
								{
									Potential.des = SimInstructions[Index].des;
								}
							}
						}


						
					}

					//TODO(ian): Change Checking function to help users know it is a generated instruction
					//from a sim instruction`

					//NOTE(ian): Error Count is a pointer!!!
					b32 ProperInstruction = IsProperInstruction(Potential, ErrorCount, i);

					if (ProperInstruction)
					{
						//TODO(ian): Check to see if InstructionCount reaches MAX_INSTRUCTIONS
						InstructionLines[(*InstructionCount)] = SimInstructionLines[Index];
						ByteCode[(*InstructionCount)++] = Potential;
					}
				}
			}
		}
	}
}

void
ProcessLabels(string ProgramBody, instruction *ByteCode, int InstructionCount, label *Labels, int *InstructionLines, int *ErrorCount, int NumLabels, int *LineLengths, int ProgramStart,int ProgramEnd)
{
	for (int i = ProgramStart; i < ProgramEnd; ++i)
	{
		if ((ByteCode[i].opcode == JMP || ByteCode[i].opcode == JE || ByteCode[i].opcode == JNE) && ByteCode[i].src == NULL_REGISTER && ByteCode[i].des == NULL_REGISTER)
		{
			//IMPORTANT TODO(ian):clean up all of the mallocs/frees


			int OperandIndex = 0;
			string Line = { 0 };
			Line.Text = &ProgramBody.Text[LineLengths[InstructionLines[i]]];
			Line.Length = LineLengths[InstructionLines[i] + 1] - LineLengths[InstructionLines[i]];
			if (ByteCode[i].opcode == JMP)
			{
				OperandIndex = FindString(Line.Text, Line.Length, "JMP", 3) + 3;
			}
			else if (ByteCode[i].opcode == JE)
			{
				OperandIndex = FindString(Line.Text, Line.Length, "JE", 2) + 2;
			}
			else if (ByteCode[i].opcode == JNE)
			{
				OperandIndex = FindString(Line.Text, Line.Length, "JNE", 3) + 3;
			}

			string Operand = { 0 };
			ParseParameters(string{ &Line.Text[OperandIndex],Line.Length - OperandIndex }, &Operand, 1);

			b32 FoundLabel = false;
			for (int j = 0; j < NumLabels; ++j)
			{
				if (EqualString(Labels[j].Name, Operand))
				{
					FoundLabel = true;
					int JumpLength = Labels[j].InstructionLine - i;
					if (JumpLength == 0)
					{
						printf("\nERROR - Line %d: This jcc instruction jumps to itself", InstructionLines[i] + 1);
						++(*ErrorCount);
						ByteCode[i].src = 1;
						ByteCode[i].des = 0;
					}
					else if (JumpLength > 0)
					{
						ByteCode[i].src = 0;
						ByteCode[i].des = JumpLength;
					}
					else
					{
						ByteCode[i].src = 1;
						ByteCode[i].des = -JumpLength;
					}
					break;
				}
			}

			if (!FoundLabel)
			{
				char Temp = Operand.Text[Operand.Length];
				Operand.Text[Operand.Length] = 0;

				printf("\nERROR - Line %d: There is no label called %s in the program.", InstructionLines[i]+1, Operand.Text);
				++(*ErrorCount);
				Operand.Text[Operand.Length] = Temp;

			}
		}
	}
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



	if (argc > 1 || 1)
	{

		FILE *f;
		fopen_s(&f,"stest.txt", "r");
		//fopen_s(&f, argv[1], "r");


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

		//printf("%s", ProgramBody.Text);


		ProgramBody = RemoveSpecialCharacters(ProgramBody);
		ToUpper(ProgramBody);
		//RemoveSpecialCharacters(ProgramBody.Text,ProgramBody.Length);

		//printf("%s", ProgramBody.Text);

		//printf("\n\n%s",ProgramBody.Text);




		//NOTE(ian): split into lines
		int NumLines = 0;
		for (int i = 0; i < ProgramBody.Length; ++i)
		{
			if (ProgramBody.Text[i] == '\n')
			{
				++NumLines;
			}
		}


		int* LineLengths = (int*)malloc((NumLines + 1) * sizeof(int));

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

		/*
		for (int i = 0; i < NumLines; ++i)
		{
		printf("\nLine #%d: ", i+1);
		for (int j = LineLengths[i]; j < LineLengths[i + 1]; ++j)
		{
		printf("%c", ProgramBody.Text[j]);
		}
		}

		int u;

		scanf_s("%d", &u);
		*/



		

		int ErrorCount = 0;

		section_marker Sections[4] = { section_marker{ -1,-1,DATA },section_marker{ -1,-1,SIM },section_marker{ -1,-1,CODE },section_marker{ -1,-1,END } };
		for (int i = 0; i < NumLines; ++i)
		{
			int DotIndex = FindString(&ProgramBody.Text[LineLengths[i]], LineLengths[i + 1] - LineLengths[i] - 1, ".", 1);
			if (DotIndex != -1)
			{
				if (FindString(&ProgramBody.Text[LineLengths[i]], LineLengths[i + 1] - LineLengths[i] - 1, ".DATA", 5) != -1)
				{
					if (Sections[0].Start == -1)
					{
						Sections[0].Start = i;
					}
				}
				else if (FindString(&ProgramBody.Text[LineLengths[i]], LineLengths[i + 1] - LineLengths[i] - 1, ".CODE", 5) != -1)
				{
					if (Sections[2].Start == -1)
					{
						Sections[2].Start = i;
					}
				}
				else if (FindString(&ProgramBody.Text[LineLengths[i]], LineLengths[i + 1] - LineLengths[i] - 1, ".SIM", 4) != -1)
				{
					if (Sections[1].Start == -1)
					{
						Sections[1].Start = i;
					}
				}
				else if (FindString(&ProgramBody.Text[LineLengths[i]], LineLengths[i + 1] - LineLengths[i] - 1, ".END", 4) != -1)
				{
					if (Sections[3].Start == -1)
					{
						Sections[3].Start = i;
					}
				}
				else
				{
					int EndHeaderIndex = DotIndex + 1;
					for (int j = EndHeaderIndex; j < LineLengths[i + 1]; ++j)
					{
						if (!AlphabeticalCharacter(ProgramBody.Text[j]))
						{
							EndHeaderIndex = j;
						}
					}

					//TODO(ian): replace temporary strings with a function!!!
					string Temp;
					Temp.Length = EndHeaderIndex - DotIndex;
					Temp.Text = (char*)malloc((Temp.Length + 1) * sizeof(char));
					for (int j = 0; j < Temp.Length; ++j)
					{
						Temp.Text[j] = ProgramBody.Text[DotIndex + j];
					}
					Temp.Text[Temp.Length] = 0;

					printf("\nERROR - Line %d: %s is not a proper header.", i + 1, Temp);
					++ErrorCount;

					free(Temp.Text);
				}
			}
		}


		for (int i = 0; i < 3; ++i)
		{
			int Min = 1000000;
			int MinIndex = -1;
			for (int j = i; j < 3; ++j)
			{
				if (Min > Sections[j].Start)
				{
					MinIndex = j;
					Min = Sections[j].Start;
				}
			}

			section_marker Temp = Sections[i];
			Sections[i] = Sections[MinIndex];
			Sections[MinIndex] = Temp;
		}

		//IMPORTANT TODO(ian): Actually find out what happens at the end of the file!!!
		//This is really hacky!!!!
		int EndSectionIndex = -1;
		for (int i = 0; i < 4; ++i)
		{
			if (Sections[i].Start != -1)
			{
				if (Sections[i].Type == END)
				{
					EndSectionIndex = i;
					break;
				}
			}
		}


		int MainMemory = MAX_MAIN_MEMORY;
		int *Queue = (int *)malloc(MAX_QUEUE_SIZE * sizeof(int));
		int QueueSize = 0;
		int QueueStart = 0;

		b32 HasSection = false;
		section_marker Section;
		for (int i = 0; i < EndSectionIndex; ++i)
		{
			if (Sections[i].Start != -1)
			{
				if (Sections[i].Type == DATA)
				{
					HasSection = true;
					Sections[i].End = Sections[i + 1].Start;
					Section = Sections[i];
					break;
				}
			}
		}


		if (HasSection) //DATA
		{
			int CompilerMode = COMPILER_NONE;
			for (int i = Section.Start + 1; i < Section.End; ++i)
			{
				if ((LineLengths[i + 1] - LineLengths[i] - 1) > 0)
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

							if (QueueSize + 1 > MAX_QUEUE_SIZE)
							{
								printf("\nERROR - Line %d: Max Queue Size Reached!!", i + 1);
								++ErrorCount;
								CompilerMode = COMPILER_NONE;
								break;
							}

							if (MainMemory == 0)
							{
								printf("\nERROR - Line %d: Main Memory is empty. There is nothing more to take.", i + 1);
								++ErrorCount;
								CompilerMode = COMPILER_NONE;
								break;
							}


							if (MainMemory - Num > 0)
							{
								Queue[QueueSize++] = Num;
								MainMemory -= Num;
							}
							else
							{
								Queue[QueueSize++] = MainMemory;
								MainMemory = 0;
							}

							j += k - j - 1;
						}
					}

					if (CompilerMode == COMPILER_NONE)
					{
						break;
					}

				}
			}
		}

		HasSection = false;
		for (int i = 0; i < EndSectionIndex; ++i)
		{
			if (Sections[i].Start != -1)
			{
				if (Sections[i].Type == SIM)
				{
					HasSection = true;
					Sections[i].End = Sections[i + 1].Start;
					Section = Sections[i];
					break;
				}
			}
		}


		int InstructionCount = 0;
		instruction* ByteCode = (instruction*)malloc(MAX_INSTRUCTIONS * sizeof(instruction));
		for (int i = 0; i < MAX_INSTRUCTIONS; ++i)
		{
			ByteCode[i] = instruction{ 0,0,0 };
		}
		//memset(ByteCode, 0, MAX_INSTRUCTIONS * sizeof(instruction));




		int *SimInstructionLines = (int *)malloc(MAX_INSTRUCTIONS * sizeof(int));



		int NumLabels = 0;
		label *Labels = (label*)malloc(MAX_LABELS * sizeof(label));

		//IMPORTANT TODO(ian): Do bounds checking to make sure these arrays do not get indexed out of range!!!
		instruction *SimInstructions = (instruction*)malloc(MAX_INSTRUCTIONS * sizeof(instruction));
		for (int i = 0; i < MAX_INSTRUCTIONS; ++i)
		{
			SimInstructions[i] = instruction{ 0,0,0 };
		}
		//memset(SimInstructions, 0, MAX_INSTRUCTIONS * sizeof(instruction));
		int MaxSimInstructions = 500;
		int NumSimInstructions = 0;
		int NumSimAssembledInstructions = 0;
		int *SimArgs = (int*)malloc(MaxSimInstructions * sizeof(int));
		string *SimNames = (string*)malloc(MaxSimInstructions * sizeof(string));

		if (HasSection) //SIM
		{
			for (int i = Section.Start+1; i < Section.End; ++i)
			{
				found_string SimInstructionStuff = HasNewSimInstruction(string{ &ProgramBody.Text[LineLengths[i]],LineLengths[i + 1] - LineLengths[i] - 1 });

				if (SimInstructionStuff.String.Length == 0 && SimInstructionStuff.Found == -1)
				{
					if(!EmptyLine(string{ &ProgramBody.Text[LineLengths[i]],LineLengths[i + 1] - LineLengths[i] - 1 }))
					{
						++ErrorCount;
						printf("\nERROR - Line %d: Expected simulated instruction name.", i + 1);
					}
				}
				else
				{
					string Parameters[MAX_PARAMETERS];
					int NumParameters = ParseParameters(string{ &ProgramBody.Text[SimInstructionStuff.Found + LineLengths[i]],LineLengths[i + 1] - SimInstructionStuff.Found - LineLengths[i]}, Parameters, MAX_PARAMETERS);

					b32 ProperOperands = true;
					for (int j = 0; j < NumParameters; ++j)
					{
						if (DetermineOperand(Parameters[j]) != NULL_REGISTER)
						{
							++ErrorCount;
							ProperOperands = false;
							printf("\nERROR - Line %d: MME, QP or R1-R%d can not be parameters for simulated instructions.", i + 1, NUM_REGISTERS);
							break;
						}
					}

					for (int j = 0; j < NumParameters - 1; ++j)
					{
						for (int k = j + 1; k < NumParameters; ++k)
						{
							if (EqualString(Parameters[j], Parameters[k]))
							{
								++ErrorCount;
								ProperOperands = false;
								char Temp = Parameters[j].Text[Parameters[j].Length];
								Parameters[j].Text[Parameters[j].Length] = 0;

								printf("\nERROR - Line %d: %s is repeated more than once", i + 1, Parameters[j]);
								Parameters[j].Text[Parameters[j].Length] = Temp;
								break;

							}
						}
					}


					if (NumParameters > MAX_PARAMETERS)
					{
						++ErrorCount;
						printf("\nERROR - Line %d: Simulated instructions can only have up to %d parameters.", i + 1,MAX_PARAMETERS);
					}


					if (ProperOperands && NumParameters <= MAX_PARAMETERS)
					{

						NumLabels = 0;

						SimNames[NumSimInstructions] = SimInstructionStuff.String;
						SimInstructions[NumSimAssembledInstructions] = instruction{ 0,0,0 };
						instruction *Head = &SimInstructions[NumSimAssembledInstructions];
						SimArgs[NumSimInstructions] = NumParameters;
						++NumSimAssembledInstructions;
						++NumSimInstructions;
						++i;

						int ProgramStart = NumSimAssembledInstructions;

						found_string NewInstruction = { 0 };
						while (i < Section.End && NewInstruction.String.Length == 0)
						{
							int Temp = NumSimAssembledInstructions;
							AssembleLine(ProgramBody, SimInstructions, &NumSimAssembledInstructions, SimInstructionLines, i, LineLengths,
								&ErrorCount, Labels, &NumLabels, SimNames, SimInstructions, &NumSimInstructions, SimInstructionLines, Parameters, NumParameters, SimArgs);

							if ((NumSimAssembledInstructions - Temp) > 0)
							{
								Head->opcode += NumSimAssembledInstructions - Temp;
							}


							++i;
							NewInstruction = HasNewSimInstruction(string{ &ProgramBody.Text[LineLengths[i]],LineLengths[i + 1] - LineLengths[i] - 1 });
						}

						/*printf("\nPrinting Labels for ");
						PrintString(SimInstructionStuff.String);
						for (int j = 0; j < NumLabels; ++j)
						{
							printf("\n");
							PrintString(Labels[j].Name);
						}*/

						ProcessLabels(ProgramBody, SimInstructions, NumSimAssembledInstructions, Labels, SimInstructionLines, &ErrorCount, NumLabels, LineLengths, ProgramStart, NumSimAssembledInstructions);
						NumLabels = 0;

						

						if (NewInstruction.String.Length != 0)
						{
							--i;
						}
					}

				}

			}
		}


		HasSection = false;
		for (int i = 0; i < EndSectionIndex; ++i)
		{
			if (Sections[i].Start != -1)
			{
				if (Sections[i].Type == CODE)
				{
					HasSection = true;
					Sections[i].End = Sections[i + 1].Start;
					Section = Sections[i];
					break;
				}
			}
		}

		int *InstructionLines = (int *)malloc(MAX_INSTRUCTIONS * sizeof(int));
		if (HasSection) //CODE
		{
			for (int i = Section.Start + 1; i < Section.End; ++i)
			{
				AssembleLine(ProgramBody, ByteCode, &InstructionCount, InstructionLines, i, LineLengths, &ErrorCount, Labels, &NumLabels, SimNames, SimInstructions, &NumSimInstructions, SimInstructionLines, 0, 0, SimArgs);
			}

			/*
			printf("\nPrinting Labels for Code");
			for (int j = 0; j < NumLabels; ++j)
			{
				printf("\n");
				PrintString(Labels[j].Name);
			}*/
			ProcessLabels(ProgramBody, ByteCode, InstructionCount, Labels, InstructionLines, &ErrorCount, NumLabels, LineLengths, 0, InstructionCount);
		}







		//PrintByteCode(ByteCode, 0, InstructionCount);

		if (ErrorCount > 0)// || 1)
		{
			if (ErrorCount == 1)
			{
				printf("\nASSEMBLY FAILED - %d error", ErrorCount);
			}
			else
			{
				printf("\nASSEMBLY FAILED - %d errors", ErrorCount);
			}
		}
		else
		{
			int *Registers = (int*)malloc(NUM_REGISTERS * sizeof(int));
			for (int i = 0; i < NUM_REGISTERS; ++i)
			{
				Registers[i] = 0;
			}

			int ProgramCounter = 0;
			int CycleCount = 0;
			int MinMainMemory = MAX_MAIN_MEMORY;
			while (ProgramCounter >= 0 && ProgramCounter < InstructionCount)
			{
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

				int ProgramIncrement = 1;
				if (ByteCode[ProgramCounter].opcode == NOP)
				{
					//NOTE(ian): Do nothing
				}
				else if (ByteCode[ProgramCounter].opcode == TFA)
				{
					/*
					MME = -2,
					QP,
					NULL_REGISTER
					*/
					if (ByteCode[ProgramCounter].src == MME)
					{
						if (ByteCode[ProgramCounter].des == MME)
						{
							//NOTE(ian): Do nothing
						}
						else if (ByteCode[ProgramCounter].des == QP)
						{
							if (QueueSize < MAX_QUEUE_SIZE)
							{
								Queue[(QueueStart + QueueSize) % MAX_QUEUE_SIZE] = MainMemory;
								MainMemory = 0;
								++QueueSize;
							}
							else
							{
								//TODO(ian): DO something!!!!!!!!!
							}
						}
						else if (ByteCode[ProgramCounter].des > NULL_REGISTER)
						{
							Registers[ByteCode[ProgramCounter].des - 1] += MainMemory;
							MainMemory = 0;
						}
					}
					else if (ByteCode[ProgramCounter].src == QP)
					{
						if (ByteCode[ProgramCounter].des == MME)
						{
							if (QueueSize > 0)
							{
								MainMemory += Queue[QueueStart];
								QueueStart = (QueueStart + 1) % MAX_QUEUE_SIZE;
								--QueueSize;
							}
							else
							{
								//TODO(ian): what should I do here?
							}
						}
						else if (ByteCode[ProgramCounter].des == QP)
						{
							if (QueueSize < MAX_QUEUE_SIZE && QueueSize > 0)
							{
								Queue[(QueueStart + QueueSize) % MAX_QUEUE_SIZE] = Queue[QueueStart];
								QueueStart = (QueueStart + 1) % MAX_QUEUE_SIZE;
								//++QueueSize;
							}
							else
							{
								//TODO(ian): DO something!!!!!!!!!
							}
						}
						else if (ByteCode[ProgramCounter].des > NULL_REGISTER)
						{
							if (QueueSize > 0)
							{
								Registers[ByteCode[ProgramCounter].des - 1] += Queue[QueueStart];
								QueueStart = (QueueStart + 1) % MAX_QUEUE_SIZE;
								--QueueSize;
							}
						}
					}
					else if (ByteCode[ProgramCounter].src > NULL_REGISTER)
					{
						if (ByteCode[ProgramCounter].des == MME)
						{
							MainMemory += Registers[ByteCode[ProgramCounter].src - 1];
							Registers[ByteCode[ProgramCounter].src - 1] = 0;
						}
						else if (ByteCode[ProgramCounter].des == QP)
						{
							if (QueueSize < MAX_QUEUE_SIZE)
							{
								Queue[(QueueStart + QueueSize) % MAX_QUEUE_SIZE] = Registers[ByteCode[ProgramCounter].src - 1];
								Registers[ByteCode[ProgramCounter].src - 1] = 0;
								++QueueSize;
							}
							else
							{
								//TODO(ian): DO something!!!!!!!!!
							}
						}
						else if (ByteCode[ProgramCounter].des > NULL_REGISTER)
						{
							Registers[ByteCode[ProgramCounter].des - 1] += Registers[ByteCode[ProgramCounter].src - 1];
							Registers[ByteCode[ProgramCounter].src - 1] = 0;
						}
					}
				}
				else if (ByteCode[ProgramCounter].opcode == TFO)
				{
					/*
					MME = -2,
					QP,
					NULL_REGISTER
					*/
					if (ByteCode[ProgramCounter].src == MME)
					{
						if (MainMemory > 0)
						{
							if (ByteCode[ProgramCounter].des == MME)
							{
								//NOTE(ian): Do nothing
							}
							else if (ByteCode[ProgramCounter].des == QP)
							{
								if (QueueSize < MAX_QUEUE_SIZE)
								{
									Queue[(QueueStart + QueueSize) % MAX_QUEUE_SIZE] = 1;
									--MainMemory;
									++QueueSize;
								}
								else
								{
									//TODO(ian): DO something!!!!!!!!!
								}
							}
							else if (ByteCode[ProgramCounter].des > NULL_REGISTER)
							{
								Registers[ByteCode[ProgramCounter].des - 1] += 1;
								--MainMemory;
							}
						}
					}
					else if (ByteCode[ProgramCounter].src == QP)
					{
						//NOTE(ian): Cannot happen!!!!!
					}
					else if (ByteCode[ProgramCounter].src > NULL_REGISTER)
					{
						if (Registers[ByteCode[ProgramCounter].src - 1] > 0)
						{
							if (ByteCode[ProgramCounter].des == MME)
							{
								MainMemory += 1;
								if (Registers[ByteCode[ProgramCounter].src - 1] > 0)
								{
									--Registers[ByteCode[ProgramCounter].src - 1];
								}
							}
							else if (ByteCode[ProgramCounter].des == QP)
							{
								if (QueueSize < MAX_QUEUE_SIZE)
								{
									Queue[(QueueStart + QueueSize) % MAX_QUEUE_SIZE] = 1;
									--Registers[ByteCode[ProgramCounter].src - 1];
									++QueueSize;
								}
								else
								{
									//TODO(ian): DO something!!!!!!!!!
								}
							}
							else if (ByteCode[ProgramCounter].des > NULL_REGISTER)
							{
								++Registers[ByteCode[ProgramCounter].des - 1];
								--Registers[ByteCode[ProgramCounter].src - 1];
							}
						}
					}
				}
				else if (ByteCode[ProgramCounter].opcode == SWP)
				{
					int temp = Registers[ByteCode[ProgramCounter].src - 1];
					Registers[ByteCode[ProgramCounter].src - 1] = Registers[ByteCode[ProgramCounter].des - 1];
					Registers[ByteCode[ProgramCounter].des - 1] = temp;
				}
				else if (ByteCode[ProgramCounter].opcode == JMP)
				{
					if (ByteCode[ProgramCounter].src == 0)
					{
						ProgramIncrement = ByteCode[ProgramCounter].des;
					}
					else
					{
						ProgramIncrement = -ByteCode[ProgramCounter].des;
					}


				}
				else if (ByteCode[ProgramCounter].opcode == JE)
				{
					if (Registers[0] == 0)
					{
						if (ByteCode[ProgramCounter].src == 0)
						{
							ProgramIncrement = ByteCode[ProgramCounter].des;
						}
						else
						{
							ProgramIncrement = -ByteCode[ProgramCounter].des;
						}
					}
				}
				else if (ByteCode[ProgramCounter].opcode == JNE)
				{
					if (Registers[0] != 0)
					{
						if (ByteCode[ProgramCounter].src == 0)
						{
							ProgramIncrement = ByteCode[ProgramCounter].des;
						}
						else
						{
							ProgramIncrement = -ByteCode[ProgramCounter].des;
						}
					}
				}
				else if (ByteCode[ProgramCounter].opcode == PRT)
				{
					if (Registers[ByteCode[ProgramCounter].src - 1] == 0)
					{
						printf("%c", (char)(Registers[ByteCode[ProgramCounter].des - 1]));
					}
					else
					{
						printf("%d", Registers[ByteCode[ProgramCounter].des - 1]);
					}
				}
				else if (ByteCode[ProgramCounter].opcode == PXL)
				{

				}
				else if (ByteCode[ProgramCounter].opcode == COL)
				{

				}
				else if (ByteCode[ProgramCounter].opcode == BRK)
				{
					printf("\n Breakpoint:\nMainMemory = %d", MainMemory);
					printf("\nRegisters: ");
					for (int j = 0; j < NUM_REGISTERS; ++j)
					{
						printf("R%d = %d", j + 1, Registers[j]);
					}
					printf("\nQueue:");
					for (int i = 0; i < QueueSize; ++i)
					{
						printf("#%d: %d | ", i, Queue[(i + QueueStart) % MAX_QUEUE_SIZE]);
					}
					int v;
					scanf_s("%d", &v);
				}
				
				ProgramCounter += ProgramIncrement;
				++CycleCount;
				if (MainMemory < MinMainMemory)
				{
					MinMainMemory = MainMemory;
				}
			}

			printf("\n\nProgram Successfully Terminated Taking %d Cycles And Using %d Units at Its Peak", CycleCount, MAX_MAIN_MEMORY - MinMainMemory);
		}
		




		int x;
		scanf_s("%d", &x);
	}


	return 0;
}