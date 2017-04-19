// 130305	Add mw3PllChipEnable(slot, 0, TRUE/FALSE) to control power
// 130519	Experientmental code for Disable HMC synt Chip


#pragma once

//---------------------------------------------------------
#pragma warning(disable:4786)
#include <stdio.h>
#include <direct.h>
#include "PulserLib.h"

#include <string>
#include <iostream>
#include <fstream>
using namespace std;

//--------------------------------------------------------
USHORT Slot;
USHORT iDev;
USHORT iReg;
USHORT NumSlots;
unsigned rval[MW3_MAX_REGS];
BOOL Refreshing;

typedef struct {
	USHORT slot;
	CardType type;
	ClockType clktype;
} SlotInfo;
SlotInfo slots[MAXSLOTS];
//----------------------------------------------------------


namespace mw3 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Globalization;
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//


			mwaveInit();	// scan the system for MW3 cards
			
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			mwaveShutDown();
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  textBox1;

	private: System::Windows::Forms::Button^  button2;


	private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::Button^  button6;
	private: System::Windows::Forms::TextBox^  textBox3;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown2;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Label^  label6;
	public: System::Windows::Forms::NumericUpDown^  numericUpDown3;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	public: 
	private: System::Windows::Forms::RadioButton^  radioButton3;
	private: System::Windows::Forms::RadioButton^  radioButton2;
	private: System::Windows::Forms::RadioButton^  radioButton1;
	private: System::Windows::Forms::Label^  label7;










	private: System::Windows::Forms::Panel^  panel4;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::TextBox^  textBox8;
	private: System::Windows::Forms::TextBox^  textBox9;





	private: System::Windows::Forms::Button^  button7;
	private: System::Windows::Forms::Button^  button13;
	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Button^  button8;

	private: System::Windows::Forms::Button^  button10;

	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label10;

	private: System::Windows::Forms::TextBox^  textBox5;
	private: System::Windows::Forms::Panel^  panel3;
	private: System::Windows::Forms::Button^  button11;

	private: System::Windows::Forms::Button^  button15;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::TextBox^  textBox6;
	private: System::Windows::Forms::TextBox^  textBox7;
	private: System::Windows::Forms::Label^  label21;
	private: System::Windows::Forms::Label^  label8;


	private: System::Windows::Forms::Label^  label19;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown7;
	private: System::Windows::Forms::Label^  label18;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown6;
	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown5;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::TextBox^  textBox4;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::Label^  label23;
private: System::Windows::Forms::Label^  label25;
private: System::Windows::Forms::Label^  label24;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Button^  button3;
private: System::Windows::Forms::Button^  button4;
private: System::Windows::Forms::Panel^  panel5;
private: System::Windows::Forms::Panel^  panel6;
private: System::Windows::Forms::Panel^  panel7;
private: System::Windows::Forms::Timer^  timer1;
private: System::Windows::Forms::PictureBox^  pictureBox1;
private: System::Windows::Forms::PictureBox^  pictureBox3;
private: System::Windows::Forms::PictureBox^  pictureBox2;
private: System::Windows::Forms::Label^  label28;
private: System::Windows::Forms::Label^  label27;
private: System::Windows::Forms::Label^  label26;
private: System::Windows::Forms::Button^  buttonReset0;
private: System::Windows::Forms::Button^  buttonReset1;
private: System::Windows::Forms::Button^  buttonReset2;
private: System::Windows::Forms::Button^  buttonEEread;
private: System::Windows::Forms::Button^  buttonEEwrite;

private: System::ComponentModel::IContainer^  components;


	public: 





	private: 


	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->textBox3 = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown2 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->buttonEEread = (gcnew System::Windows::Forms::Button());
			this->buttonEEwrite = (gcnew System::Windows::Forms::Button());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->textBox4 = (gcnew System::Windows::Forms::TextBox());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->radioButton3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown3 = (gcnew System::Windows::Forms::NumericUpDown());
			this->panel4 = (gcnew System::Windows::Forms::Panel());
			this->buttonReset0 = (gcnew System::Windows::Forms::Button());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
			this->panel5 = (gcnew System::Windows::Forms::Panel());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown7 = (gcnew System::Windows::Forms::NumericUpDown());
			this->button13 = (gcnew System::Windows::Forms::Button());
			this->button7 = (gcnew System::Windows::Forms::Button());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->textBox8 = (gcnew System::Windows::Forms::TextBox());
			this->textBox9 = (gcnew System::Windows::Forms::TextBox());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->buttonReset1 = (gcnew System::Windows::Forms::Button());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->panel6 = (gcnew System::Windows::Forms::Panel());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown6 = (gcnew System::Windows::Forms::NumericUpDown());
			this->button8 = (gcnew System::Windows::Forms::Button());
			this->button10 = (gcnew System::Windows::Forms::Button());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->textBox5 = (gcnew System::Windows::Forms::TextBox());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->buttonReset2 = (gcnew System::Windows::Forms::Button());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->panel7 = (gcnew System::Windows::Forms::Panel());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown5 = (gcnew System::Windows::Forms::NumericUpDown());
			this->button11 = (gcnew System::Windows::Forms::Button());
			this->button15 = (gcnew System::Windows::Forms::Button());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->textBox6 = (gcnew System::Windows::Forms::TextBox());
			this->textBox7 = (gcnew System::Windows::Forms::TextBox());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->BeginInit();
			this->panel1->SuspendLayout();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown3))->BeginInit();
			this->panel4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown7))->BeginInit();
			this->panel2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown6))->BeginInit();
			this->panel3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown5))->BeginInit();
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->BackColor = System::Drawing::Color::Beige;
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->textBox1->Location = System::Drawing::Point(42, 495);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox1->Size = System::Drawing::Size(406, 172);
			this->textBox1->TabIndex = 0;
			this->textBox1->TextChanged += gcnew System::EventHandler(this, &Form1::textBox1_TextChanged);
			// 
			// button2
			// 
			this->button2->Enabled = false;
			this->button2->Location = System::Drawing::Point(421, 24);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(65, 68);
			this->button2->TabIndex = 2;
			this->button2->Text = L"Register Dump";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->numericUpDown1->Location = System::Drawing::Point(30, 51);
			this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->numericUpDown1->Size = System::Drawing::Size(59, 29);
			this->numericUpDown1->TabIndex = 5;
			this->numericUpDown1->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDown1->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown1_ValueChanged);
			// 
			// textBox2
			// 
			this->textBox2->BackColor = System::Drawing::SystemColors::Info;
			this->textBox2->Font = (gcnew System::Drawing::Font(L"Courier New", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->textBox2->ForeColor = System::Drawing::Color::Blue;
			this->textBox2->Location = System::Drawing::Point(229, 24);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(100, 29);
			this->textBox2->TabIndex = 6;
			this->textBox2->Text = L"000000";
			this->textBox2->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->textBox2->TextChanged += gcnew System::EventHandler(this, &Form1::textBox2_TextChanged);
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(353, 24);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(51, 28);
			this->button5->TabIndex = 7;
			this->button5->Text = L"Write";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &Form1::button5_Click);
			// 
			// button6
			// 
			this->button6->Location = System::Drawing::Point(353, 65);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(51, 28);
			this->button6->TabIndex = 9;
			this->button6->Text = L"Read";
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Click += gcnew System::EventHandler(this, &Form1::button6_Click);
			// 
			// textBox3
			// 
			this->textBox3->Font = (gcnew System::Drawing::Font(L"Courier New", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->textBox3->ForeColor = System::Drawing::Color::BlueViolet;
			this->textBox3->Location = System::Drawing::Point(229, 63);
			this->textBox3->Name = L"textBox3";
			this->textBox3->Size = System::Drawing::Size(100, 29);
			this->textBox3->TabIndex = 8;
			this->textBox3->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(33, 34);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(41, 13);
			this->label1->TabIndex = 10;
			this->label1->Text = L"Device";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(260, 6);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(34, 13);
			this->label2->TabIndex = 11;
			this->label2->Text = L"Value";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(120, 33);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(46, 13);
			this->label3->TabIndex = 13;
			this->label3->Text = L"Register";
			// 
			// numericUpDown2
			// 
			this->numericUpDown2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->numericUpDown2->Hexadecimal = true;
			this->numericUpDown2->Location = System::Drawing::Point(120, 51);
			this->numericUpDown2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {31, 0, 0, 0});
			this->numericUpDown2->Name = L"numericUpDown2";
			this->numericUpDown2->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->numericUpDown2->Size = System::Drawing::Size(59, 29);
			this->numericUpDown2->TabIndex = 12;
			this->numericUpDown2->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDown2->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown2_ValueChanged);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(207, 72);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(18, 13);
			this->label4->TabIndex = 14;
			this->label4->Text = L"0x";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(208, 32);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(18, 13);
			this->label5->TabIndex = 15;
			this->label5->Text = L"0x";
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::Lavender;
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel1->Controls->Add(this->buttonEEread);
			this->panel1->Controls->Add(this->buttonEEwrite);
			this->panel1->Controls->Add(this->label22);
			this->panel1->Controls->Add(this->label5);
			this->panel1->Controls->Add(this->label4);
			this->panel1->Controls->Add(this->label3);
			this->panel1->Controls->Add(this->numericUpDown2);
			this->panel1->Controls->Add(this->label2);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Controls->Add(this->button2);
			this->panel1->Controls->Add(this->button6);
			this->panel1->Controls->Add(this->numericUpDown1);
			this->panel1->Controls->Add(this->textBox3);
			this->panel1->Controls->Add(this->button5);
			this->panel1->Controls->Add(this->textBox2);
			this->panel1->Enabled = false;
			this->panel1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->panel1->Location = System::Drawing::Point(42, 373);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(594, 105);
			this->panel1->TabIndex = 16;
			// 
			// buttonEEread
			// 
			this->buttonEEread->Location = System::Drawing::Point(499, 57);
			this->buttonEEread->Name = L"buttonEEread";
			this->buttonEEread->Size = System::Drawing::Size(75, 23);
			this->buttonEEread->TabIndex = 18;
			this->buttonEEread->Text = L"PROMRead";
			this->buttonEEread->UseVisualStyleBackColor = true;
			this->buttonEEread->Click += gcnew System::EventHandler(this, &Form1::buttonEEread_Click);
			// 
			// buttonEEwrite
			// 
			this->buttonEEwrite->Location = System::Drawing::Point(500, 24);
			this->buttonEEwrite->Name = L"buttonEEwrite";
			this->buttonEEwrite->Size = System::Drawing::Size(75, 23);
			this->buttonEEwrite->TabIndex = 17;
			this->buttonEEwrite->Text = L"PROM Write";
			this->buttonEEwrite->UseVisualStyleBackColor = true;
			this->buttonEEwrite->Click += gcnew System::EventHandler(this, &Form1::buttonEEwrite_Click);
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->BackColor = System::Drawing::Color::Lavender;
			this->label22->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label22->Font = (gcnew System::Drawing::Font(L"Arial", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label22->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label22->Location = System::Drawing::Point(7, 2);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(108, 20);
			this->label22->TabIndex = 16;
			this->label22->Text = L"Register View";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label7->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label7->Location = System::Drawing::Point(562, 654);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(70, 13);
			this->label7->TabIndex = 20;
			this->label7->Text = L"FPGA  VERS";
			// 
			// groupBox1
			// 
			this->groupBox1->BackColor = System::Drawing::Color::Lavender;
			this->groupBox1->Controls->Add(this->button1);
			this->groupBox1->Controls->Add(this->textBox4);
			this->groupBox1->Controls->Add(this->label21);
			this->groupBox1->Controls->Add(this->label8);
			this->groupBox1->Controls->Add(this->radioButton3);
			this->groupBox1->Controls->Add(this->radioButton2);
			this->groupBox1->Controls->Add(this->radioButton1);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(475, 495);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(161, 156);
			this->groupBox1->TabIndex = 19;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Ref Clock";
			// 
			// button1
			// 
			this->button1->ForeColor = System::Drawing::Color::Blue;
			this->button1->Location = System::Drawing::Point(85, 117);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(52, 28);
			this->button1->TabIndex = 23;
			this->button1->Text = L"Apply";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click_1);
			// 
			// textBox4
			// 
			this->textBox4->Font = (gcnew System::Drawing::Font(L"Consolas", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->textBox4->ForeColor = System::Drawing::Color::Red;
			this->textBox4->Location = System::Drawing::Point(30, 119);
			this->textBox4->Name = L"textBox4";
			this->textBox4->Size = System::Drawing::Size(47, 26);
			this->textBox4->TabIndex = 22;
			this->textBox4->Text = L"100";
			this->textBox4->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(39, 104);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(29, 13);
			this->label21->TabIndex = 21;
			this->label21->Text = L"MHz";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(436, 20);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(29, 13);
			this->label8->TabIndex = 4;
			this->label8->Text = L"MHz";
			// 
			// radioButton3
			// 
			this->radioButton3->AutoSize = true;
			this->radioButton3->Location = System::Drawing::Point(12, 49);
			this->radioButton3->Name = L"radioButton3";
			this->radioButton3->Size = System::Drawing::Size(42, 17);
			this->radioButton3->TabIndex = 2;
			this->radioButton3->Text = L"PXI";
			this->radioButton3->UseVisualStyleBackColor = true;
			this->radioButton3->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton3_CheckedChanged);
			// 
			// radioButton2
			// 
			this->radioButton2->AutoSize = true;
			this->radioButton2->Location = System::Drawing::Point(12, 72);
			this->radioButton2->Name = L"radioButton2";
			this->radioButton2->Size = System::Drawing::Size(63, 17);
			this->radioButton2->TabIndex = 1;
			this->radioButton2->Text = L"External";
			this->radioButton2->UseVisualStyleBackColor = true;
			this->radioButton2->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton2_CheckedChanged);
			// 
			// radioButton1
			// 
			this->radioButton1->AutoSize = true;
			this->radioButton1->Checked = true;
			this->radioButton1->Location = System::Drawing::Point(12, 26);
			this->radioButton1->Name = L"radioButton1";
			this->radioButton1->Size = System::Drawing::Size(60, 17);
			this->radioButton1->TabIndex = 0;
			this->radioButton1->TabStop = true;
			this->radioButton1->Text = L"Internal";
			this->radioButton1->UseVisualStyleBackColor = true;
			this->radioButton1->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton1_CheckedChanged_1);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(48, 22);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(41, 20);
			this->label6->TabIndex = 18;
			this->label6->Text = L"Slot\r\n";
			this->label6->Click += gcnew System::EventHandler(this, &Form1::label6_Click);
			// 
			// numericUpDown3
			// 
			this->numericUpDown3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->numericUpDown3->ForeColor = System::Drawing::Color::Blue;
			this->numericUpDown3->Location = System::Drawing::Point(101, 17);
			this->numericUpDown3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {18, 0, 0, 0});
			this->numericUpDown3->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
			this->numericUpDown3->Name = L"numericUpDown3";
			this->numericUpDown3->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->numericUpDown3->Size = System::Drawing::Size(59, 29);
			this->numericUpDown3->TabIndex = 17;
			this->numericUpDown3->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDown3->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
			this->numericUpDown3->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown3_ValueChanged);
			// 
			// panel4
			// 
			this->panel4->BackColor = System::Drawing::Color::PaleGoldenrod;
			this->panel4->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel4->Controls->Add(this->buttonReset0);
			this->panel4->Controls->Add(this->label28);
			this->panel4->Controls->Add(this->pictureBox3);
			this->panel4->Controls->Add(this->panel5);
			this->panel4->Controls->Add(this->label25);
			this->panel4->Controls->Add(this->label19);
			this->panel4->Controls->Add(this->numericUpDown7);
			this->panel4->Controls->Add(this->button13);
			this->panel4->Controls->Add(this->button7);
			this->panel4->Controls->Add(this->label14);
			this->panel4->Controls->Add(this->label15);
			this->panel4->Controls->Add(this->label16);
			this->panel4->Controls->Add(this->textBox8);
			this->panel4->Controls->Add(this->textBox9);
			this->panel4->Location = System::Drawing::Point(42, 264);
			this->panel4->Name = L"panel4";
			this->panel4->Size = System::Drawing::Size(594, 82);
			this->panel4->TabIndex = 25;
			// 
			// buttonReset0
			// 
			this->buttonReset0->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->buttonReset0->Location = System::Drawing::Point(477, 30);
			this->buttonReset0->Name = L"buttonReset0";
			this->buttonReset0->Size = System::Drawing::Size(49, 33);
			this->buttonReset0->TabIndex = 38;
			this->buttonReset0->Text = L"Reset";
			this->buttonReset0->UseVisualStyleBackColor = true;
			this->buttonReset0->Click += gcnew System::EventHandler(this, &Form1::buttonReset0_Click);
			// 
			// label28
			// 
			this->label28->AutoSize = true;
			this->label28->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label28->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label28->Location = System::Drawing::Point(3, 9);
			this->label28->Name = L"label28";
			this->label28->Size = System::Drawing::Size(24, 16);
			this->label28->TabIndex = 37;
			this->label28->Text = L"Ch\r\n";
			// 
			// pictureBox3
			// 
			this->pictureBox3->BackColor = System::Drawing::Color::Black;
			this->pictureBox3->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pictureBox3->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox3.Image")));
			this->pictureBox3->InitialImage = nullptr;
			this->pictureBox3->Location = System::Drawing::Point(556, 37);
			this->pictureBox3->Name = L"pictureBox3";
			this->pictureBox3->Size = System::Drawing::Size(18, 16);
			this->pictureBox3->TabIndex = 36;
			this->pictureBox3->TabStop = false;
			this->pictureBox3->Visible = false;
			// 
			// panel5
			// 
			this->panel5->BackColor = System::Drawing::Color::Black;
			this->panel5->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel5->Location = System::Drawing::Point(558, 37);
			this->panel5->Name = L"panel5";
			this->panel5->Size = System::Drawing::Size(17, 15);
			this->panel5->TabIndex = 32;
			// 
			// label25
			// 
			this->label25->AutoSize = true;
			this->label25->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label25->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label25->Location = System::Drawing::Point(61, 60);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(77, 13);
			this->label25->TabIndex = 31;
			this->label25->Text = L"25 - 6000 MHz";
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label19->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label19->Location = System::Drawing::Point(276, 11);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(48, 16);
			this->label19->TabIndex = 30;
			this->label19->Text = L"Ref Div";
			// 
			// numericUpDown7
			// 
			this->numericUpDown7->Font = (gcnew System::Drawing::Font(L"Consolas", 14.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->numericUpDown7->ForeColor = System::Drawing::Color::Red;
			this->numericUpDown7->Location = System::Drawing::Point(279, 31);
			this->numericUpDown7->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown7->Name = L"numericUpDown7";
			this->numericUpDown7->Size = System::Drawing::Size(51, 30);
			this->numericUpDown7->TabIndex = 31;
			this->numericUpDown7->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDown7->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// button13
			// 
			this->button13->Enabled = false;
			this->button13->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button13->ForeColor = System::Drawing::Color::Blue;
			this->button13->Location = System::Drawing::Point(353, 27);
			this->button13->Name = L"button13";
			this->button13->Size = System::Drawing::Size(51, 33);
			this->button13->TabIndex = 28;
			this->button13->Text = L"Apply";
			this->button13->UseVisualStyleBackColor = true;
			this->button13->Click += gcnew System::EventHandler(this, &Form1::button13_Click);
			// 
			// button7
			// 
			this->button7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button7->Location = System::Drawing::Point(421, 29);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(45, 33);
			this->button7->TabIndex = 26;
			this->button7->Text = L"Off";
			this->button7->UseVisualStyleBackColor = true;
			this->button7->Click += gcnew System::EventHandler(this, &Form1::button7_Click);
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label14->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label14->Location = System::Drawing::Point(195, 11);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(60, 16);
			this->label14->TabIndex = 25;
			this->label14->Text = L"Attn (dB)";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label15->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label15->Location = System::Drawing::Point(61, 9);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(73, 16);
			this->label15->TabIndex = 24;
			this->label15->Text = L"Freq (MHz)";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->BackColor = System::Drawing::SystemColors::ControlLight;
			this->label16->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label16->Font = (gcnew System::Drawing::Font(L"Arial", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label16->ForeColor = System::Drawing::Color::Blue;
			this->label16->Location = System::Drawing::Point(6, 32);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(20, 21);
			this->label16->TabIndex = 23;
			this->label16->Text = L"0";
			// 
			// textBox8
			// 
			this->textBox8->BackColor = System::Drawing::SystemColors::HighlightText;
			this->textBox8->Font = (gcnew System::Drawing::Font(L"Consolas", 14.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->textBox8->ForeColor = System::Drawing::Color::Red;
			this->textBox8->Location = System::Drawing::Point(187, 29);
			this->textBox8->Name = L"textBox8";
			this->textBox8->Size = System::Drawing::Size(80, 30);
			this->textBox8->TabIndex = 22;
			this->textBox8->Text = L"0.00";
			this->textBox8->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBox9
			// 
			this->textBox9->BackColor = System::Drawing::SystemColors::HighlightText;
			this->textBox9->Font = (gcnew System::Drawing::Font(L"Consolas", 14.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->textBox9->ForeColor = System::Drawing::Color::Red;
			this->textBox9->Location = System::Drawing::Point(36, 27);
			this->textBox9->Name = L"textBox9";
			this->textBox9->Size = System::Drawing::Size(141, 30);
			this->textBox9->TabIndex = 21;
			this->textBox9->Text = L"2000.000000";
			this->textBox9->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// panel2
			// 
			this->panel2->BackColor = System::Drawing::Color::PaleGoldenrod;
			this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel2->Controls->Add(this->buttonReset1);
			this->panel2->Controls->Add(this->label27);
			this->panel2->Controls->Add(this->pictureBox2);
			this->panel2->Controls->Add(this->panel6);
			this->panel2->Controls->Add(this->label24);
			this->panel2->Controls->Add(this->label18);
			this->panel2->Controls->Add(this->numericUpDown6);
			this->panel2->Controls->Add(this->button8);
			this->panel2->Controls->Add(this->button10);
			this->panel2->Controls->Add(this->label9);
			this->panel2->Controls->Add(this->label10);
			this->panel2->Controls->Add(this->textBox5);
			this->panel2->Location = System::Drawing::Point(42, 159);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(594, 82);
			this->panel2->TabIndex = 26;
			// 
			// buttonReset1
			// 
			this->buttonReset1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->buttonReset1->Location = System::Drawing::Point(477, 29);
			this->buttonReset1->Name = L"buttonReset1";
			this->buttonReset1->Size = System::Drawing::Size(49, 33);
			this->buttonReset1->TabIndex = 37;
			this->buttonReset1->Text = L"Reset";
			this->buttonReset1->UseVisualStyleBackColor = true;
			this->buttonReset1->Click += gcnew System::EventHandler(this, &Form1::buttonReset1_Click);
			// 
			// label27
			// 
			this->label27->AutoSize = true;
			this->label27->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label27->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label27->Location = System::Drawing::Point(3, 9);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(24, 16);
			this->label27->TabIndex = 36;
			this->label27->Text = L"Ch\r\n";
			// 
			// pictureBox2
			// 
			this->pictureBox2->BackColor = System::Drawing::Color::Black;
			this->pictureBox2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pictureBox2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox2.Image")));
			this->pictureBox2->InitialImage = nullptr;
			this->pictureBox2->Location = System::Drawing::Point(556, 36);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(18, 16);
			this->pictureBox2->TabIndex = 35;
			this->pictureBox2->TabStop = false;
			this->pictureBox2->Visible = false;
			// 
			// panel6
			// 
			this->panel6->BackColor = System::Drawing::Color::Black;
			this->panel6->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel6->Location = System::Drawing::Point(557, 36);
			this->panel6->Name = L"panel6";
			this->panel6->Size = System::Drawing::Size(17, 15);
			this->panel6->TabIndex = 33;
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label24->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label24->Location = System::Drawing::Point(54, 62);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(92, 13);
			this->label24->TabIndex = 30;
			this->label24->Text = L"7300  - 8200 MHz";
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label18->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label18->Location = System::Drawing::Point(274, 11);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(48, 16);
			this->label18->TabIndex = 30;
			this->label18->Text = L"Ref Div";
			// 
			// numericUpDown6
			// 
			this->numericUpDown6->Font = (gcnew System::Drawing::Font(L"Consolas", 14.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->numericUpDown6->ForeColor = System::Drawing::Color::Red;
			this->numericUpDown6->Location = System::Drawing::Point(276, 31);
			this->numericUpDown6->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown6->Name = L"numericUpDown6";
			this->numericUpDown6->Size = System::Drawing::Size(51, 30);
			this->numericUpDown6->TabIndex = 31;
			this->numericUpDown6->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDown6->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// button8
			// 
			this->button8->Enabled = false;
			this->button8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button8->ForeColor = System::Drawing::Color::Blue;
			this->button8->Location = System::Drawing::Point(353, 27);
			this->button8->Name = L"button8";
			this->button8->Size = System::Drawing::Size(51, 32);
			this->button8->TabIndex = 28;
			this->button8->Text = L"Apply";
			this->button8->UseVisualStyleBackColor = true;
			this->button8->Click += gcnew System::EventHandler(this, &Form1::button8_Click);
			// 
			// button10
			// 
			this->button10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button10->Location = System::Drawing::Point(421, 29);
			this->button10->Name = L"button10";
			this->button10->Size = System::Drawing::Size(45, 32);
			this->button10->TabIndex = 26;
			this->button10->Text = L"Off";
			this->button10->UseVisualStyleBackColor = true;
			this->button10->Click += gcnew System::EventHandler(this, &Form1::button10_Click);
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label9->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label9->Location = System::Drawing::Point(61, 9);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(73, 16);
			this->label9->TabIndex = 24;
			this->label9->Text = L"Freq (MHz)";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->BackColor = System::Drawing::SystemColors::ControlLight;
			this->label10->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label10->Font = (gcnew System::Drawing::Font(L"Arial", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label10->ForeColor = System::Drawing::Color::Blue;
			this->label10->Location = System::Drawing::Point(6, 31);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(20, 21);
			this->label10->TabIndex = 23;
			this->label10->Text = L"1";
			// 
			// textBox5
			// 
			this->textBox5->BackColor = System::Drawing::SystemColors::HighlightText;
			this->textBox5->Font = (gcnew System::Drawing::Font(L"Consolas", 14.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->textBox5->ForeColor = System::Drawing::Color::Red;
			this->textBox5->Location = System::Drawing::Point(36, 27);
			this->textBox5->Name = L"textBox5";
			this->textBox5->Size = System::Drawing::Size(141, 30);
			this->textBox5->TabIndex = 21;
			this->textBox5->Text = L"7374.000000";
			this->textBox5->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// panel3
			// 
			this->panel3->BackColor = System::Drawing::Color::PaleGoldenrod;
			this->panel3->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel3->Controls->Add(this->buttonReset2);
			this->panel3->Controls->Add(this->label26);
			this->panel3->Controls->Add(this->pictureBox1);
			this->panel3->Controls->Add(this->panel7);
			this->panel3->Controls->Add(this->label20);
			this->panel3->Controls->Add(this->label17);
			this->panel3->Controls->Add(this->numericUpDown5);
			this->panel3->Controls->Add(this->button11);
			this->panel3->Controls->Add(this->button15);
			this->panel3->Controls->Add(this->label11);
			this->panel3->Controls->Add(this->label12);
			this->panel3->Controls->Add(this->label13);
			this->panel3->Controls->Add(this->textBox6);
			this->panel3->Controls->Add(this->textBox7);
			this->panel3->Location = System::Drawing::Point(42, 61);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(594, 82);
			this->panel3->TabIndex = 27;
			// 
			// buttonReset2
			// 
			this->buttonReset2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->buttonReset2->Location = System::Drawing::Point(477, 28);
			this->buttonReset2->Name = L"buttonReset2";
			this->buttonReset2->Size = System::Drawing::Size(49, 33);
			this->buttonReset2->TabIndex = 36;
			this->buttonReset2->Text = L"Reset";
			this->buttonReset2->UseVisualStyleBackColor = true;
			this->buttonReset2->Click += gcnew System::EventHandler(this, &Form1::buttonReset2_Click);
			// 
			// label26
			// 
			this->label26->AutoSize = true;
			this->label26->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label26->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label26->Location = System::Drawing::Point(5, 8);
			this->label26->Name = L"label26";
			this->label26->Size = System::Drawing::Size(24, 16);
			this->label26->TabIndex = 35;
			this->label26->Text = L"Ch\r\n";
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::Black;
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->InitialImage = nullptr;
			this->pictureBox1->Location = System::Drawing::Point(556, 36);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(18, 16);
			this->pictureBox1->TabIndex = 34;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Visible = false;
			// 
			// panel7
			// 
			this->panel7->BackColor = System::Drawing::Color::Black;
			this->panel7->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel7->Location = System::Drawing::Point(557, 37);
			this->panel7->Name = L"panel7";
			this->panel7->Size = System::Drawing::Size(17, 15);
			this->panel7->TabIndex = 33;
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label20->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label20->Location = System::Drawing::Point(58, 60);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(77, 13);
			this->label20->TabIndex = 29;
			this->label20->Text = L"25 - 6000 MHz";
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label17->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label17->Location = System::Drawing::Point(274, 10);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(48, 16);
			this->label17->TabIndex = 28;
			this->label17->Text = L"Ref Div";
			// 
			// numericUpDown5
			// 
			this->numericUpDown5->Font = (gcnew System::Drawing::Font(L"Consolas", 14.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->numericUpDown5->ForeColor = System::Drawing::Color::Red;
			this->numericUpDown5->Location = System::Drawing::Point(276, 30);
			this->numericUpDown5->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown5->Name = L"numericUpDown5";
			this->numericUpDown5->Size = System::Drawing::Size(51, 30);
			this->numericUpDown5->TabIndex = 29;
			this->numericUpDown5->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDown5->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// button11
			// 
			this->button11->Enabled = false;
			this->button11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button11->ForeColor = System::Drawing::Color::Blue;
			this->button11->Location = System::Drawing::Point(353, 28);
			this->button11->Name = L"button11";
			this->button11->Size = System::Drawing::Size(51, 31);
			this->button11->TabIndex = 28;
			this->button11->Text = L"Apply";
			this->button11->UseVisualStyleBackColor = true;
			this->button11->Click += gcnew System::EventHandler(this, &Form1::button11_Click);
			// 
			// button15
			// 
			this->button15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button15->Location = System::Drawing::Point(421, 28);
			this->button15->Name = L"button15";
			this->button15->Size = System::Drawing::Size(45, 33);
			this->button15->TabIndex = 26;
			this->button15->Text = L"Off";
			this->button15->UseVisualStyleBackColor = true;
			this->button15->Click += gcnew System::EventHandler(this, &Form1::button15_Click);
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label11->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label11->Location = System::Drawing::Point(195, 10);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(60, 16);
			this->label11->TabIndex = 25;
			this->label11->Text = L"Attn (dB)";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label12->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
			this->label12->Location = System::Drawing::Point(61, 8);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(73, 16);
			this->label12->TabIndex = 24;
			this->label12->Text = L"Freq (MHz)";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->BackColor = System::Drawing::SystemColors::ControlLight;
			this->label13->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label13->Font = (gcnew System::Drawing::Font(L"Arial", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label13->ForeColor = System::Drawing::Color::Blue;
			this->label13->Location = System::Drawing::Point(8, 32);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(20, 21);
			this->label13->TabIndex = 23;
			this->label13->Text = L"2";
			// 
			// textBox6
			// 
			this->textBox6->BackColor = System::Drawing::SystemColors::HighlightText;
			this->textBox6->Font = (gcnew System::Drawing::Font(L"Consolas", 14.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->textBox6->ForeColor = System::Drawing::Color::Red;
			this->textBox6->Location = System::Drawing::Point(187, 28);
			this->textBox6->Name = L"textBox6";
			this->textBox6->Size = System::Drawing::Size(80, 30);
			this->textBox6->TabIndex = 22;
			this->textBox6->Text = L"10.00";
			this->textBox6->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBox7
			// 
			this->textBox7->BackColor = System::Drawing::SystemColors::HighlightText;
			this->textBox7->Font = (gcnew System::Drawing::Font(L"Consolas", 14.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->textBox7->ForeColor = System::Drawing::Color::Red;
			this->textBox7->Location = System::Drawing::Point(36, 27);
			this->textBox7->Name = L"textBox7";
			this->textBox7->Size = System::Drawing::Size(141, 30);
			this->textBox7->TabIndex = 21;
			this->textBox7->Text = L"2000.000000";
			this->textBox7->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label23
			// 
			this->label23->AutoSize = true;
			this->label23->Font = (gcnew System::Drawing::Font(L"Wingdings", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(2)));
			this->label23->Location = System::Drawing::Point(320, 350);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(17, 12);
			this->label23->TabIndex = 28;
			this->label23->Text = L"a";
			// 
			// button3
			// 
			this->button3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button3->ForeColor = System::Drawing::Color::Blue;
			this->button3->Location = System::Drawing::Point(398, 12);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(50, 43);
			this->button3->TabIndex = 30;
			this->button3->Text = L"Apply All";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click_2);
			// 
			// button4
			// 
			this->button4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button4->Location = System::Drawing::Point(465, 12);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(45, 43);
			this->button4->TabIndex = 29;
			this->button4->Text = L"Off All";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &Form1::button4_Click_1);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 200;
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(670, 718);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->label23);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->panel4);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->numericUpDown3);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->textBox1);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"Form1";
			this->Text = L"MAGIQWAVE 4083";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown3))->EndInit();
			this->panel4->ResumeLayout(false);
			this->panel4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown7))->EndInit();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown6))->EndInit();
			this->panel3->ResumeLayout(false);
			this->panel3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown5))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	
//------------------------------------------------------------------------

	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
				}

	private: System::Void textBox2_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				 // Register value to write
				 String ^s;
				 UInt32 regVal;
				 try {
					regVal = System::UInt32::Parse(textBox2->Text, NumberStyles::HexNumber);
				 } catch (...) { return; }

				 if (regVal > 0xffffff) {
					 s = "ERROR: Bad register value: " + regVal.ToString("X") + "\r\n";
					 textBox1->AppendText(s);
					 button5->Enabled = false;
					 return;
				 }
				 button5->Enabled = true;
			 }

	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {}

	private: System::Void mwaveInit() {
				 Refreshing = FALSE;
				 // Search for devices button.
				 USHORT slot = 0;
				 USHORT dev = 0;
				 NumSlots = pulserInit();
				 if (NumSlots ==0) {
					 textBox1->AppendText("No device found.\r\n");
					 return;
				 }
				 else 
					 textBox1->AppendText("device(s) found.\r\n");

				 Slot = 0;

				 for (USHORT k = 0; k < MAXSLOTS; k++) {
					 slot = k + 1;
					 CardType type = pulserGetCardType(slot);
					 if (type == Mw3Type) { 
						 slots[k].slot = slot;
						 slots[k].type = type;
						 if (Slot == 0) {	// determine the first slot
							 Slot = slot;
						 }
						 textBox1->AppendText("Slot " + slot.ToString() + ": " + STR_MW3 + "\r\n");
						 mw3Reset(slot);
						 mw3Init(slot);
						 for (dev = 0; dev < MW3_MAX_CHANNELS; dev++) {
							 mw3SetRefDivider(slot, dev, 1);
						 }
//						 mw3SetOutputFreq(slot, 0, 2105.0);
//						 mw3SetOutputFreq(slot, 1, 7374.0);
//						 mw3SetOutputFreq(slot, 2, 3060.0);
						 mw3SetGUIFreq(slot, 0, 2105.0);
						 mw3SetGUIFreq(slot, 1, 7374.0);
						 mw3SetGUIFreq(slot, 2, 3060.0);
						 mw3SetAtten(slot, 0, 0.0);
						 mw3SetAtten(slot, 2, 0.0);
						 // turn off output (shouldn't need this)
						 mw3OutputOff(slot, 0);
						 mw3OutputOff(slot, 1);
						 mw3OutputOff(slot, 2);
						 // select internal clock
						 mw3SelectClock(slot, ClockInternal, 0);
						 // turn on output
						 //mw3OutputOn(slot, 0);
						 //mw3OutputOn(slot, 1);
						 //mw3OutputOn(slot, 2);
					 }
				 }

				 // Set the GUI for the first slot
				 numericUpDown3->Value = Slot;
//				if		(Slot == 0) textBox1->AppendText("Slot 0.\r\n");
//				else if (Slot == 1) textBox1->AppendText("Slot 1.\r\n");

//				 numericUpDown3->Value = 2;
				 mwaveRefreshGui();
				 Refreshing = FALSE;

				// Clear register data
				for (int i = 0; i < MW3_MAX_REGS; i++)
					 rval[i] = 0;
				return;
		}

	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
				 regDump(iDev);
			 }

	private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			}

	private: System::Void numericUpDown1_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 // New device number
			 iDev = numericUpDown1->Value.ToInt32(numericUpDown1->Value);
			 textBox1->AppendText("Device = " + iDev.ToString() + "\r\n");
			 //selectSlave(iDev);
			 if (iDev < MW3_MAX_CHANNELS) {
				 button2->Enabled = true;
				 button6->Enabled = true;
				 numericUpDown2->Enabled = true;
			 }
			 else {
				 button2->Enabled = false;
				 button6->Enabled = false;
				 numericUpDown2->Enabled = false;
			 }
		 }

	private: System::Void numericUpDown2_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			// New register number
			iReg = numericUpDown2->Value.ToInt32(numericUpDown2->Value);
			//textBox1->AppendText("Register = " + iReg.ToString("X") + "\r\n");
		}

	private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {
			 // Load button
			 }

	private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {
			// Write a register button
			int regVal;
			System::String ^s = textBox2->Text;
			try {
				regVal = System::Int32::Parse(s, NumberStyles::HexNumber);
			} 
			catch (...) {
				textBox1->AppendText("**** Invalid input " + s + "\r\n");
				return;
			}
			if (regVal > 0x00ffffff) {
				s = "**** Bad value: " + regVal.ToString("X") + "\r\n";
				textBox1->AppendText(s);
				return;
			}
			mw3WriteRegister(Slot, iDev, iReg, regVal);
			textBox2->Text = regVal.ToString("X6");
		}

	private: System::Void button6_Click(System::Object^  sender, System::EventArgs^  e) {
			 // Read a register button
			 iReg = numericUpDown2->Value.ToInt32(numericUpDown2->Value);
			 int val = mw3ReadRegister(Slot, iDev, iReg);
			 textBox3->Text = val.ToString("X6");
		 }
	
	private: void regDump (USHORT dev) {
			textBox1->AppendText("\r\nRegister Dump (Dev " + dev.ToString() + "):\r\n");
			String ^s;
			for (int i = 0; i < MW3_MAX_REGS; i++)  {
				if (i % 4 == 0)
					s = i.ToString("X2") + ": ";
				UInt32 v = mw3ReadRegister(Slot, dev, i);	
				s += v.ToString("X6") + " ";
				if (i % 4 == 3)
					textBox1->AppendText(s + "\r\n");
			}
		}

	private: void setSlot(USHORT slot) {
				 Slot = slot;
		}

	private: void enableGUI(bool enable) {	// enable = TRUE to enable, FALSE to disable
				 // register view panel
				 button2->Enabled = enable;
				 button5->Enabled = enable;
				 button6->Enabled = enable;
				 panel1->Enabled = enable;
				 groupBox1->Enabled = enable;
				 // reference clock group
				 button1->Enabled = enable;
				 radioButton1->Enabled = enable;
				 radioButton2->Enabled = enable;
				 radioButton3->Enabled = enable;
				 textBox4->Enabled = enable;
				 if (!radioButton2->Checked)
					 textBox4->Enabled = false;
				 // channel 2
				 textBox7->Enabled = enable;
				 textBox6->Enabled = enable;
				 numericUpDown5->Enabled = enable;
				 button11->Enabled = enable;
				 button15->Enabled = enable;
				 // channel 1
				 textBox5->Enabled = enable;
				 numericUpDown6->Enabled = enable;
				 button8->Enabled = enable;
				 button10->Enabled = enable;
				 // channel 0
				 textBox9->Enabled = enable;
				 textBox8->Enabled = enable;
				 numericUpDown7->Enabled = enable;
				 button13->Enabled = enable;
				 button7->Enabled = enable;
				 // 'ALL' buttons
				 button3->Enabled = enable;
				 button4->Enabled = enable;
		}

private: System::Void label6_Click(System::Object^  sender, System::EventArgs^  e) {
		 }

private: System::Void numericUpDown3_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				USHORT newslot = numericUpDown3->Value.ToInt32(numericUpDown3->Value);
				setSlot(newslot);
				if (slots[newslot-1].type == Mw3Type)
					mwaveRefreshGui();
				else 
					enableGUI(false);		
		 }

private: System::Void radioButton1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		
		 }
private: System::Void label8_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void radioButton1_CheckedChanged_1(System::Object^  sender, System::EventArgs^  e) {
			// Clock source changed to INTERNAL
			 if (radioButton1->Checked) {
				mw3SelectClock(Slot, ClockInternal, 0);
				ULONG clockMhz = (ULONG)mw3GetClockMHz(Slot);
				textBox1->AppendText("[" + Slot.ToString() + "]" + "Ref. clock: INTERNAL " + clockMhz.ToString() + " MHz.\r\n");
				textBox4->Text = clockMhz.ToString();
				textBox4->Enabled = FALSE;
				button1->Enabled = FALSE;
				if (Refreshing) {
					//textBox1->AppendText("Refreshing\r\n");
					Refreshing = FALSE;
				}
				else { 
					mwaveOutputOff(Slot, 0);
					mwaveOutputOff(Slot, 1);
					mwaveOutputOff(Slot, 2);
				}
			 }
		 }

private: System::Void radioButton3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 // Clock source changed to PXI10
			 if (radioButton3->Checked) {
				 mw3SelectClock(Slot, ClockPxi, 0);
				 ULONG clockMhz = (ULONG)mw3GetClockMHz(Slot);
				 textBox1->AppendText("[" + Slot.ToString() + "]" + "Ref. clock: PXI " + clockMhz.ToString() + " MHz.\r\n");
				 textBox4->Text = clockMhz.ToString();
				 textBox4->Enabled = FALSE;
				 button1->Enabled = FALSE;
				 if (Refreshing) {
					textBox1->AppendText("Refreshing\r\n");
					 Refreshing = FALSE;
				 }
				 else { 
					 mwaveOutputOff(Slot, 0);
//			 mwaveOutputOff(Slot, 1);
					 mwaveOutputOff(Slot, 2);
				 }
			 }
		 }

private: System::Void radioButton2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			// Clock source changed to EXTERNAL
			 if (radioButton2->Checked) {
				double clockFreq = mw3GetClockMHz(Slot);
				textBox1->AppendText("[" + Slot.ToString() + "]" + " EXTERNAL clock:\r\n");
				textBox1->AppendText("Please enter clock frequency >>>\r\n");
				textBox4->Text = "";
				textBox4->Enabled = TRUE;
				button1->Enabled = TRUE;
			}
		 }

private: System::Void button11_Click(System::Object^  sender, System::EventArgs^  e) {
			 // Apply button for channel 2. 
			 // The frequency is in textBox7; attenuation in textbox6.
			 CultureInfo^ MyCI = gcnew CultureInfo( "en-US",false );
			 NumberFormatInfo^ nfi = MyCI->NumberFormat;
			 nfi->NumberGroupSeparator = "";
	
			 USHORT channel = 2;
			 System::String^ freqText = textBox7->Text;
			 System::String^ attnText = textBox6->Text;
			 System::Decimal divText = numericUpDown5->Value;

			  mwaveApply(Slot, channel, freqText, attnText, divText);

			  nfi->NumberDecimalDigits = 6;
			  textBox7->Text = mw3GetOutputFreq(Slot, channel).ToString("F", nfi);
			  nfi->NumberDecimalDigits = 2;
			  textBox6->Text = mw3GetAtten(Slot, channel).ToString("F", nfi);
		 }

private: System::Void button8_Click(System::Object^  sender, System::EventArgs^  e) {
			 // Apply button for channel 1 (hmc764); 
			 // the frequency is in textbox5 and the divider is ref. div. 6.
			 CultureInfo^ MyCI = gcnew CultureInfo( "en-US",false );
			 NumberFormatInfo^ nfi = MyCI->NumberFormat;
			 nfi->NumberGroupSeparator = "";

			 USHORT channel = 1;
			 System::String^ freqText = textBox5->Text;
			 System::String^ attnText = textBox5->Text;
			 System::Decimal divText = numericUpDown6->Value;
			 mwaveApply(Slot, channel, freqText, attnText, divText);
			 nfi->NumberDecimalDigits = 6;
			 textBox5->Text = mw3GetOutputFreq(Slot, channel).ToString("F", nfi);	
		}

private: System::Void button13_Click(System::Object^  sender, System::EventArgs^  e) {
			 // Apply button for device 0 (hmc833); 
			 // the frequency is in textbox9 and the attenuation is in textbox8.
			 CultureInfo^ MyCI = gcnew CultureInfo( "en-US",false );
			 NumberFormatInfo^ nfi = MyCI->NumberFormat;
			 nfi->NumberGroupSeparator = "";
			 USHORT channel = 0;
			 System::String^ freqText = textBox9->Text;
			 System::String^ attnText = textBox8->Text;
			 System::Decimal divText = numericUpDown7->Value;
			 mwaveApply(Slot, channel, freqText, attnText, divText);
			 nfi->NumberDecimalDigits = 6;
			 textBox9->Text = mw3GetOutputFreq(Slot, channel).ToString("F", nfi);
			 nfi->NumberDecimalDigits = 2;
			 textBox8->Text = mw3GetAtten(Slot, channel).ToString("F", nfi);
		 }

 private: System::Void button15_Click(System::Object^  sender, System::EventArgs^  e) {
			// Dev 2 output OFF button
			mwaveOutputOff(Slot, 2);
		}

private: System::Void button10_Click(System::Object^  sender, System::EventArgs^  e) {
			 // Dev 1 output OFF button
			 mwaveOutputOff(Slot, 1);
		 }

private: System::Void button7_Click(System::Object^  sender, System::EventArgs^  e) {
			 // Dev 0 output OFF button
			 mwaveOutputOff(Slot, 0);
		 }
		
private: System::Void mwaveApply(USHORT slot, USHORT channel, \
			 System::String^ freqText, System::String^ attnText, System::Decimal divValue)
		 {
			 ULONG refDivider;
			 double freq, freqMin, freqMax;
			 BOOL r;

//			mw3PllChipEnable(slot, channel, TRUE);

			 // get reference divider
			refDivider = divValue.ToInt32(divValue);

			 //  get frequency
			 try {
				 freq = System::Double::Parse(freqText, NumberStyles::Float);
			 } catch (...) {
				 textBox1->AppendText("ERROR: Invalid frequency value.\r\n");
				 return; 
			 }
			 // Check for bounds
			 if (channel == 1) {
				 freqMin = 7300.0;
				 freqMax = 8200.0;
			 }
			 else {
				 freqMin = 25.0;
				 freqMax = 6000.0;
			 }
			 if ((freq < freqMin) || (freq > freqMax)) {
				 textBox1->AppendText("ERROR: Frequency out of range.\r\n");
				 return;
			 }
			 mw3SetOutputFreq(slot, channel, freq);
			 // Now the attenuator
			 double loss = 0.0;	// in DB (positive)
			 if (channel != 1) {
				try {
					loss = System::Double::Parse(attnText, NumberStyles::Float);
				} catch (...) {
					textBox1->AppendText("ERROR: Invalid attenuation value.\r\n");
					return; 
				}
				// Check for bounds
				if ((loss < 0) || (loss > MW3_ATTEN_LOSS_MAX)) {
					textBox1->AppendText("Attenuation out of range (0 - 15.75 dB).\r\n");
					return;
				}
				// Set the attenuation
				r = mw3SetAtten(slot, channel, loss);
				if (!r)
					textBox1->AppendText("[" + Slot.ToString() + "-" + channel.ToString() + "] " + \
							"Failed to set attenuation to " + loss.ToString() + " dB.\r\n");
			 }

			 // Set the reference divider
			 mw3SetRefDivider(slot, channel, refDivider);

			 // turn on the output
			 r = mw3OutputOn(Slot, channel);
			 // Set the frequency
			 // r = mw3SetOutputFreq(slot, channel, freq);
			 if (r) {
				 // turn on the output
				 textBox1->AppendText("[" + Slot.ToString() + "-" + channel.ToString() + "] " + \
							freq.ToString() + " MHz, " + loss.ToString() + " dB\r\n");
			 }
			 else
				 textBox1->AppendText("Failed to set frequency to " + freq.ToString() + " MHz.\r\n");

			 return;
		 }


private: System::Void mwaveOutputOff(USHORT slot, USHORT dev) {
			 textBox1->AppendText("[" + slot.ToString() + "-" + dev.ToString() + "] Output OFF.\r\n");
			  mw3OutputOff(slot, dev);
		 }

private:  System::Void mwaveShutDown(Void) {
			   USHORT slot;
			   for (USHORT k = 0; k < MAXSLOTS; k++) {
				   slot = k + 1;
				   CardType type = pulserGetCardType(slot);
				   if (type ==Mw3Type) { 
						mw3OutputOff(slot, 0);
						mw3OutputOff(slot, 1);
						mw3OutputOff(slot, 2);
//						mw3PllChipEnable(slot, 0, FALSE);
//						mw3PllChipEnable(slot, 0, FALSE);
//						mw3PllChipEnable(slot, 0, FALSE);

//						mw3ShutDown(slot);
				   }
			   }
		  }

private: System::Void button1_Click_1(System::Object^  sender, System::EventArgs^  e) {
			 // APPLY button for the external reference clock.
			 // The frequency text is in textbox4.

			 UInt32 extClockMhz;
			 try {
				 extClockMhz = System::UInt32::Parse(textBox4->Text, NumberStyles::Integer);
			 } catch (...) {
				 textBox1->AppendText("Invalid frequency: " + textBox4->Text + "\r\n");
				 return; 
			 }

			 mw3SelectClock(Slot, ClockExternal, extClockMhz);
			 textBox1->AppendText("**EXTERNAL clock: " + extClockMhz.ToString() + " MHz.\r\n");
			 mwaveOutputOff(Slot, 0);
			 mwaveOutputOff(Slot, 1);
			 mwaveOutputOff(Slot, 2);
		 }


private: System::Void mwaveRefreshGui() {
			 USHORT channel, refDiv;
			 ClockType refClock;
			 double freq, attn, clockFreq;
			 CultureInfo^ MyCI = gcnew CultureInfo( "en-US",false );
			 NumberFormatInfo^ nfi = MyCI->NumberFormat;
			 nfi->NumberGroupSeparator = "";
		
			 // Reconstruct the GUI from stored values.

			 // Reference clock type and frequency
			 refClock = mw3GetClockType(Slot);
			 clockFreq = mw3GetClockMHz(Slot);
			 nfi->NumberDecimalDigits = 0;
			 textBox4->Text = clockFreq.ToString("F", nfi);
			 Refreshing = TRUE;
			
			 if (refClock == ClockInternal) {
				 radioButton1->Checked = TRUE;
				 radioButton3->Checked = FALSE;
				 radioButton2->Checked = FALSE;
			 }
			 else if (refClock == ClockPxi) {
				 radioButton1->Checked = FALSE;
				 radioButton3->Checked = TRUE;
				 radioButton2->Checked = FALSE;
			 }
			 else {	// ClockExternal
				 radioButton1->Checked = FALSE;
				 radioButton3->Checked = FALSE;
				 radioButton2->Checked = TRUE;
				 textBox4->Text = clockFreq.ToString();
			 }

			 // firmware version
			 ULONG vers = pulserGetFwVersion(Slot);
			 String ^versStr = String::Format("FW: {0:X4}", vers );
			 label7->Text = versStr;
			 
			 // Channel panes
			 for (channel = 0; channel < MW3_MAX_CHANNELS; channel++) {
				 freq = mw3GetOutputFreq(Slot, channel);
				 attn = mw3GetAtten(Slot, channel);
				 refDiv = (USHORT)mw3GetRefDivider(Slot, channel);
				 textBox1->AppendText("[" + Slot.ToString() + "-" + channel.ToString() + 
				 		"] Get freq = " + freq.ToString() + "\r\n");

				 if (channel == 0) {
					 nfi->NumberDecimalDigits = 6;
					 textBox9->Text = freq.ToString("F", nfi);
					 nfi->NumberDecimalDigits = 2;
					 textBox8->Text = attn.ToString("F", nfi);
					 numericUpDown7->Value = refDiv;
				 }
				 else if (channel == 1) {
					 nfi->NumberDecimalDigits = 6;
					 nfi->NumberGroupSeparator = "";
					 textBox5->Text = freq.ToString("F", nfi);
					 numericUpDown6->Value = refDiv;
				}
				 else {
					 nfi->NumberDecimalDigits = 6;
					 textBox7->Text = freq.ToString("F", nfi);
					 nfi->NumberDecimalDigits = 2;
					 textBox6->Text = attn.ToString("F", nfi);
					 numericUpDown5->Value = refDiv;
				}
			 }
			 enableGUI(TRUE);
		 }
/*
private: System::Void calcR19(void) {
			 // calculate R09
			 double R = 1.0;
			 double clockMHz = 10.0;
			 double freqMHz = 7374.0;
			 double Icp = 0.002;	// Icp = B19
			 double B18 = 1.0/(freqMHz * 1.0e6);
			 double B20 = 1.0e6 * clockMHz / R;
			 //double B21 = (0.4e-8 + 4 * B18) * Icp * B20;
			 double B22 = (0.4e-8 + 4 * B18) * Icp * B20 / 0.000055;
			 ULONG u = (ULONG)(B22 + 0.5);
			 if (u > 7)
				 u = 7;
			 double J42  = (Icp > 0.002)? 0.002/0.0005 : Icp/0.0005;
			 ULONG j42 = (ULONG)J42 & 0x7;
			 ULONG R9 = (u << 7) | (j42 << 21) | (j42 << 18);
		 }
*/
private: System::Void button3_Click_2(System::Object^  sender, System::EventArgs^  e) {
			 // Apply All button
			 // send an "apply" event for all channels
			 button13_Click(this, e);
			 button8_Click(this, e);
			 button11_Click(this, e);
		 }

private: System::Void button4_Click_1(System::Object^  sender, System::EventArgs^  e) {
			 // Off All button
			 // Send an "Off" event for all channels
			 button7_Click(this, e);
			 button10_Click(this, e);
			 button15_Click(this, e);
		 }

private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
			 BOOL mw3Type = (pulserGetCardType(Slot) == Mw3Type)? TRUE : FALSE;
			 // channel 0
			 if (mw3Type && mw3Locked(Slot, 0)) {
				pictureBox3->Visible = true;
			 }
			else {
				pictureBox3->Visible = false;
			}
			// channel 1
			if (mw3Type && mw3Locked(Slot, 1)) {
				pictureBox2->Visible = true;
			}
			else {
				pictureBox2->Visible = false;
				//panel6->BackColor = System::Drawing::Color::PaleGoldenrod;
			}
			// channel 2
			if ((pulserGetCardType(Slot) == Mw3Type) && mw3Locked(Slot, 2)) {
				pictureBox1->Visible = true;
			}
			else {
				pictureBox1->Visible = false;
			}
		 }

private: System::Void buttonReset2_Click(System::Object^  sender, System::EventArgs^  e) {
			 textBox1->AppendText("[" + Slot + "-2" + "] PLL reset\r\n");
			 mw3ResetPll(Slot, 2);
		 }
private: System::Void buttonReset1_Click(System::Object^  sender, System::EventArgs^  e) {
			 textBox1->AppendText("[" + Slot + "-1" + "] PLL reset\r\n");
			 mw3ResetPll(Slot, 1);
		 }
private: System::Void buttonReset0_Click(System::Object^  sender, System::EventArgs^  e) {
			 textBox1->AppendText("[" + Slot + "-0" + "] PLL reset\r\n");
			 mw3ResetPll(Slot, 0);
		 }

private: System::Void buttonEEwrite_Click(System::Object^  sender, System::EventArgs^  e) {
			 UCHAR buffer[16];
			 UCHAR x = numericUpDown2->Value.ToByte(numericUpDown2->Value);
			 int i;
			 for (i = 0; i < 16; i++) {
				 buffer[i] = x + i;
			 }
			 if (mw3WriteEeProm(Slot, 0, sizeof(buffer), buffer)) {
				textBox1->AppendText("[" + Slot + "] " + "WriteEeProm:\r\n");
				for (i = 0; i < sizeof(buffer); i++)
					textBox1->AppendText(buffer[i] + " ");
				textBox1->AppendText("\r\n");
			 }
			 else
				 textBox1->AppendText("EEPROM write failed\r\n");
		 }
private: System::Void buttonEEread_Click(System::Object^  sender, System::EventArgs^  e) {
			 UCHAR buffer[16];
			 int i;
			 for (i = 0; i < sizeof(buffer); i++)
				 buffer[i] = 0;
			 if(mw3ReadEeProm(Slot, 0, sizeof(buffer), buffer)) {
				textBox1->AppendText("[" + Slot + "] " + "ReadEeProm:\r\n");
				for (i = 0; i < sizeof(buffer); i++)
					textBox1->AppendText(buffer[i] + " ");
				textBox1->AppendText("\r\n");
			 }
			 else
				 textBox1->AppendText("EEPROM read failed\r\n");
		 }
private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
}; //--class Form1
} //--namespace mw3
