#include "MyForm.h"
#include <cmath>


using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute] //Атрибут для main
int main(array<String^>^ args) //^ - вместо delete
{
    Application::SetCompatibleTextRenderingDefault(false); //Обработа тескта
    Application::EnableVisualStyles(); //Обработка визуальных стилей 
    Kepler12::MyForm form; //Обращение к классу и создание элемента form
    Application::Run(% form); // Обращение к методу Run, передача по ссылке form
}


double Kepler12::MyForm::f(double x, double y)
{
    return -M * G * x / pow(x * x + y * y, 1.5);
}

double Kepler12::MyForm::g(double x, double y)
{
    return -M * G * y / pow(x * x + y * y, 1.5);
}

void Kepler12::MyForm::DrawEarth() //Отрисовка Земли, круг прорисовыается через полярные координаты, где r = радиус Земли
{
    this->chart1->Series[1]->Points->Clear();

    R_Earth = 6400000;
    for (int i = 0; i <= 360; i++)
    {
        angle = i * 4 * atan(1) / 180;
        double x = R_Earth * cos(angle);
        double y = R_Earth * sin(angle);
        this->chart1->Series[1]->Points->AddXY(x, y);
    }
}

System::Void Kepler12::MyForm::buttonA_Click(System::Object^ sender, System::EventArgs^ e)
{
    AA = true;
}

System::Void Kepler12::MyForm::buttonB_Click(System::Object^ sender, System::EventArgs^ e)
{
    BB = true;
}

System::Void Kepler12::MyForm::buttonC_Click(System::Object^ sender, System::EventArgs^ e)
{
    CC = true;
    DD = true;
}


void Kepler12::MyForm::RK()
{
    DrawEarth();
    n = (int)(tmax / dt) + 2;
    xx = new double[n + 1];
    yy = new double[n + 1];
    vxx = new double[n + 1];
    vyy = new double[n + 1];

    xx[0] = x0 * 1000;
    yy[0] = 0;
    vyy[0] = vy0;
    vxx[0] = 0;
    G = 6.673e-11;
    M = 6e24;
    AA, BB, CC, DD = false;
    a, b, e = 0;
    double kx[4], ky[4];
    int i = 0;
    double t = 0;

    do
    {
        t = i * dt;
        kx[0] = f(xx[i], yy[i]) * dt;
        ky[0] = g(xx[i], yy[i]) * dt;

        kx[1] = f(xx[i] + vxx[i] * dt / 2., yy[i] + vyy[i] * dt / 2.) * dt;
        ky[1] = g(xx[i] + vxx[i] * dt / 2., yy[i] + vyy[i] * dt / 2.) * dt;

        kx[2] = f(xx[i] + vxx[i] * dt / 2. + kx[0] * dt / 4, yy[i] + vyy[i] * dt / 2. + ky[0] * dt / 4) * dt;
        ky[2] = g(xx[i] + vxx[i] * dt / 2. + kx[0] * dt / 4, yy[i] + vyy[i] * dt / 2. + ky[0] * dt / 4) * dt;

        kx[3] = f(xx[i] + vxx[i] * dt + kx[1] * dt / 2., yy[i] + vyy[i] * dt + ky[1] * dt / 2.) * dt;
        ky[3] = g(xx[i] + vxx[i] * dt + kx[1] * dt / 2., yy[i] + vyy[i] * dt + ky[1] * dt / 2.) * dt;

        xx[i + 1] = xx[i] + vxx[i] * dt + (kx[0] + kx[1] + kx[2]) * dt / 6.;
        yy[i + 1] = yy[i] + vyy[i] * dt + (ky[0] + ky[1] + ky[2]) * dt / 6.;

        vxx[i + 1] = vxx[i] + (kx[0] + 2 * kx[1] + 2 * kx[2] + kx[3]) / 6.;
        vyy[i + 1] = vyy[i] + (ky[0] + 2 * ky[1] + 2 * ky[2] + ky[3]) / 6;

        if (AA)
        {
            Ax = xx[i];
            Ay = yy[i];
            ta = t;
            this->chart1->Series[2]->Points->AddXY(Ax, Ay);
            AA = false;
        }

        if (BB)
        {
            Bx = xx[i];
            By = yy[i];
            tb = t;
            this->chart1->Series[2]->Points->AddXY(Bx, By);
            BB = false;
        }

        if (CC)
        {
            Cx = xx[i];
            Cy = yy[i];
            tc = t;
            this->chart1->Series[2]->Points->AddXY(Cx, Cy);
            CC = false;
        }

        if (t >= (tc + tb-ta) && tc!=0 && tb != 0 && ta != 0 && DD)
        {
            Dx = xx[i];
            Dy = yy[i];
            td = t;
            this->chart1->Series[2]->Points->AddXY(Dx, Dy);
            DD = false;
        }

        // Площадь треугольников напрямую через координаты
        if (tb != ta)
        {
            Sab = (0.5 * abs(Ax * By - Ay * Bx)) / (tb - ta);
        }
        if (td != tc)
        {
            Scd = (0.5 * abs(Cx * Dy - Cy * Dx)) / (td - tc);
        }

        textBox1->Text = Convert::ToString(Sab);
        textBox2->Text = Convert::ToString(Scd);

        Ek = 0.5 * (vxx[i + 1] * vxx[i + 1] + vyy[i + 1] * vyy[i + 1]) * 1e-7;
        Ep = -G * M / sqrt(xx[i + 1] * xx[i + 1] + yy[i + 1] * yy[i + 1]) * 1e-7;
        E = Ek + Ep;

        if (vyy[i + 1] * vyy[i] < 0)
        {
            a = abs(x0 * 1000 - xx[i + 1]);
            b = abs(yy[i + 1]);
            e = sqrt(1 - b * b / (a * a));
            textBox_e->Text = Convert::ToString((int(1000 * e)) / 1000.);
        }

        if (a == 0)
        {
            this->chart1->Series[0]->Points->AddXY(xx[i], yy[i]);
        }
        if (a != 0 && i % 2 == 0)
        {
            this->chart1->Series[0]->Points->AddXY(xx[i], yy[i]);
        }
       
        if (i % 10 == 0)
        {
            // Графики энергий и момента импульса
            this->chart2->Series[0]->Points->AddXY(t, Ek);
            this->chart3->Series[0]->Points->AddXY(t, Ep);
            this->chart4->Series[0]->Points->AddXY(t, E);
        }
        if (stop) break;
        Application::DoEvents();
        i++;
    } while (t <= tmax && !stop);
}



//Функция при нажатии кнопки старт
System::Void Kepler12::MyForm::button1_Click(System::Object^ sender, System::EventArgs^ e)
{
    // Проверка на заполненность полей
    if (textBox_x0->Text == "" || textBox_vy0->Text == "" || textBox_dt->Text == "" || textBox_t_max->Text == "")
    {
        MessageBox::Show("Все поля должны быть заполнены перед запуском!", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    stop = false;
    // Преобразование введенных данных
    x0 = Convert::ToInt32(textBox_x0->Text);
    vy0 = Convert::ToInt32(textBox_vy0->Text);
    dt = Convert::ToDouble(textBox_dt->Text);
    tmax = Convert::ToInt32(textBox_t_max->Text);

    // Опрделение границ графиков орбиты, энергий, момента импульса
    chart1->ChartAreas[0]->AxisX->Maximum = 1e7;
    chart1->ChartAreas[0]->AxisX->Minimum = -6e7;
    chart1->ChartAreas[0]->AxisY->Maximum = 2e7;
    chart1->ChartAreas[0]->AxisY->Minimum = -2e7;

    chart2->ChartAreas[0]->AxisX->Maximum = tmax;
    chart2->ChartAreas[0]->AxisX->Minimum = 0;

    chart3->ChartAreas[0]->AxisX->Maximum = tmax;
    chart3->ChartAreas[0]->AxisX->Minimum = 0;

    chart4->ChartAreas[0]->AxisX->Maximum = tmax;
    chart4->ChartAreas[0]->AxisX->Minimum = 0;

    // Очистка графиков перед отрисовкой
    this->chart1->Series[0]->Points->Clear();
    this->chart2->Series[0]->Points->Clear();
    this->chart3->Series[0]->Points->Clear();
    this->chart4->Series[0]->Points->Clear();

    // Очистка полей в форме перед отрисовкой
    textBox_e->Text = "";

    // Очистка динамических массивов перед отрисовкой 
    delete[] xx;
    delete[] yy;
    delete[] vxx;
    delete[] vyy;

    // Запуск рассчетов и отрисовки
    RK();
}

//Функция при нажатии кнопки стоп
System::Void Kepler12::MyForm::button2_Click(System::Object^ sender, System::EventArgs^ e)
{
    stop = true;
}