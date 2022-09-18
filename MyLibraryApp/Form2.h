#pragma once
#include "Other.h"
#include<Windows.h>
namespace MyLibraryApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	using namespace System::Diagnostics;

	/// <summary>
	/// Summary for Form2
	/// </summary>
	public ref class Form2 : public System::Windows::Forms::Form
	{
	public:
		Form2(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			books = gcnew array<String^> {"Sherlock", "Dracula", "Speech", "Wishes", "Scarlet", "Happy", "Falling", "Legend", "Hopeless", "Trouble"};
			avail = (int*) calloc (10, sizeof(int));
			for(int i = 0; i < 10; i++)
				avail[i] = 5;
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form2()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: array< String^ >^ books;
	//private: array< Int32^ >^ avail;
	int *avail;
	private: System::Windows::Forms::PictureBox^  pictureBox1;

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form2::typeid));
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->BackColor = System::Drawing::SystemColors::Info;
			this->button1->Location = System::Drawing::Point(221, 152);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(173, 34);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Record Command";
			this->button1->UseVisualStyleBackColor = false;
			this->button1->Click += gcnew System::EventHandler(this, &Form2::button1_Click);
			// 
			// textBox1
			// 
			this->textBox1->BackColor = System::Drawing::SystemColors::ControlLight;
			this->textBox1->Cursor = System::Windows::Forms::Cursors::AppStarting;
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Segoe Print", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->textBox1->Location = System::Drawing::Point(221, 12);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox1->Size = System::Drawing::Size(398, 134);
			this->textBox1->TabIndex = 1;
			this->textBox1->Text = L"Welcome!!\r\nClick on Record Command and Speak \'Search\' to start finding a book\r\n";
			this->textBox1->TextChanged += gcnew System::EventHandler(this, &Form2::textBox1_TextChanged);
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->pictureBox1->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(239, 192);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(188, 267);
			this->pictureBox1->TabIndex = 2;
			this->pictureBox1->TabStop = false;
			// 
			// Form2
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(691, 496);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->button1);
			this->Name = L"Form2";
			this->Text = L"Search";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {	
				Other^ obj = gcnew Other();
				if (obj->live_testing(2) == 0){
					textBox1->Text = "Speak the name of the book";
					this->Refresh();
					Sleep(2000);
					int output = obj->live_testing(9);
					String^ predictedBookName = books[output];
					pictureBox1->Image = Image::FromFile(String::Concat("Images\\project\\", predictedBookName + ".jpg"));  
					textBox1->Text = predictedBookName + "\r\nAvailable: " + avail[output] + "\r\nReturn  or Borrow?";

					this->Refresh();
					Sleep(5000);
					output = obj->live_testing(2);
					if(output == 1){
						avail[output] ++;
						textBox1->Text = "Book Returned Successfully!\r\n Current Availability: " + avail[output];
					}else if(output == 2){
						avail[output] -- ;
						textBox1->Text = "You have borrowed " + predictedBookName + "!\r\n Current Availability: " + avail[output];
					}else{
						textBox1->Text = "Didn't understand. Please try again.\r\nClick on Record Command and Speak 'Search' to start finding a book";
					}
				}else{
					textBox1->Text = "Command not found. Please try again!\r\nClick on Record Command and Speak 'Search' to start finding a book";
				}
			 }

	private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			 }
	};
}
