#include "CalculatorWidget.h"
#include <AK/Assertions.h>
#include <LibGUI/GButton.h>
#include <LibGUI/GLabel.h>
#include <LibGUI/GTextBox.h>

CalculatorWidget::CalculatorWidget(GWidget* parent)
    : GWidget(parent)
{
    set_fill_with_background_color(true);

    m_entry = GTextBox::construct(this);
    m_entry->set_relative_rect(5, 5, 244, 26);
    m_entry->set_text_alignment(TextAlignment::CenterRight);

    m_label = GLabel::construct(this);
    m_label->set_relative_rect(12, 42, 27, 27);
    m_label->set_foreground_color(Color::NamedColor::Red);
    m_label->set_frame_shadow(FrameShadow::Sunken);
    m_label->set_frame_shape(FrameShape::Container);
    m_label->set_frame_thickness(2);

    update_display();

    for (int i = 0; i < 10; i++) {
        m_digit_button[i] = GButton::construct(this);
        auto& button = *m_digit_button[i];
        int p = i ? i + 2 : 0;
        int x = 55 + (p % 3) * 39;
        int y = 177 - (p / 3) * 33;
        button.move_to(x, y);
        button.set_foreground_color(Color::NamedColor::Blue);
        add_button(button, i);
    }

    m_mem_add_button = GButton::construct(this);
    m_mem_add_button->move_to(9, 177);
    m_mem_add_button->set_foreground_color(Color::NamedColor::Red);
    m_mem_add_button->set_text("M+");
    add_button(*m_mem_add_button, Calculator::Operation::MemAdd);

    m_mem_save_button = GButton::construct(this);
    m_mem_save_button->move_to(9, 144);
    m_mem_save_button->set_foreground_color(Color::NamedColor::Red);
    m_mem_save_button->set_text("MS");
    add_button(*m_mem_save_button, Calculator::Operation::MemSave);

    m_mem_recall_button = GButton::construct(this);
    m_mem_recall_button->move_to(9, 111);
    m_mem_recall_button->set_foreground_color(Color::NamedColor::Red);
    m_mem_recall_button->set_text("MR");
    add_button(*m_mem_recall_button, Calculator::Operation::MemRecall);

    m_mem_clear_button = GButton::construct(this);
    m_mem_clear_button->move_to(9, 78);
    m_mem_clear_button->set_foreground_color(Color::NamedColor::Red);
    m_mem_clear_button->set_text("MC");
    add_button(*m_mem_clear_button, Calculator::Operation::MemClear);

    m_clear_button = GButton::construct(this);
    m_clear_button->set_foreground_color(Color::NamedColor::Red);
    m_clear_button->set_text("C");
    m_clear_button->on_click = [this](GButton&) {
        m_keypad.set_value(0.0);
        m_calculator.clear_operation();
        update_display();
    };
    add_button(*m_clear_button);
    m_clear_button->set_relative_rect(187, 40, 60, 28);

    m_clear_error_button = GButton::construct(this);
    m_clear_error_button->set_foreground_color(Color::NamedColor::Red);
    m_clear_error_button->set_text("CE");
    m_clear_error_button->on_click = [this](GButton&) {
        m_calculator.clear_error();
        update_display();
    };
    add_button(*m_clear_error_button);
    m_clear_error_button->set_relative_rect(124, 40, 59, 28);

    m_backspace_button = GButton::construct(this);
    m_backspace_button->set_foreground_color(Color::NamedColor::Red);
    m_backspace_button->set_text("Backspace");
    m_backspace_button->on_click = [this](GButton&) {
        m_keypad.type_backspace();
        update_display();
    };
    add_button(*m_backspace_button);
    m_backspace_button->set_relative_rect(55, 40, 65, 28);

    m_decimal_point_button = GButton::construct(this);
    m_decimal_point_button->move_to(133, 177);
    m_decimal_point_button->set_foreground_color(Color::NamedColor::Blue);
    m_decimal_point_button->set_text(".");
    m_decimal_point_button->on_click = [this](GButton&) {
        m_keypad.type_decimal_point();
        update_display();
    };
    add_button(*m_decimal_point_button);

    m_sign_button = GButton::construct(this);
    m_sign_button->move_to(94, 177);
    m_sign_button->set_foreground_color(Color::NamedColor::Blue);
    m_sign_button->set_text("+/-");
    add_button(*m_sign_button, Calculator::Operation::ToggleSign);

    m_add_button = GButton::construct(this);
    m_add_button->move_to(172, 177);
    m_add_button->set_foreground_color(Color::NamedColor::Red);
    m_add_button->set_text("+");
    add_button(*m_add_button, Calculator::Operation::Add);

    m_subtract_button = GButton::construct(this);
    m_subtract_button->move_to(172, 144);
    m_subtract_button->set_foreground_color(Color::NamedColor::Red);
    m_subtract_button->set_text("-");
    add_button(*m_subtract_button, Calculator::Operation::Subtract);

    m_multiply_button = GButton::construct(this);
    m_multiply_button->move_to(172, 111);
    m_multiply_button->set_foreground_color(Color::NamedColor::Red);
    m_multiply_button->set_text("*");
    add_button(*m_multiply_button, Calculator::Operation::Multiply);

    m_divide_button = GButton::construct(this);
    m_divide_button->move_to(172, 78);
    m_divide_button->set_foreground_color(Color::NamedColor::Red);
    m_divide_button->set_text("/");
    add_button(*m_divide_button, Calculator::Operation::Divide);

    m_sqrt_button = GButton::construct(this);
    m_sqrt_button->move_to(211, 78);
    m_sqrt_button->set_foreground_color(Color::NamedColor::Blue);
    m_sqrt_button->set_text("sqrt");
    add_button(*m_sqrt_button, Calculator::Operation::Sqrt);

    m_inverse_button = GButton::construct(this);
    m_inverse_button->move_to(211, 144);
    m_inverse_button->set_foreground_color(Color::NamedColor::Blue);
    m_inverse_button->set_text("1/x");
    add_button(*m_inverse_button, Calculator::Operation::Inverse);

    m_percent_button = GButton::construct(this);
    m_percent_button->move_to(211, 111);
    m_percent_button->set_foreground_color(Color::NamedColor::Blue);
    m_percent_button->set_text("%");
    add_button(*m_percent_button, Calculator::Operation::Percent);

    m_equals_button = GButton::construct(this);
    m_equals_button->move_to(211, 177);
    m_equals_button->set_foreground_color(Color::NamedColor::Red);
    m_equals_button->set_text("=");
    m_equals_button->on_click = [this](GButton&) {
        double argument = m_keypad.value();
        double res = m_calculator.finish_operation(argument);
        m_keypad.set_value(res);
        update_display();
    };
    add_button(*m_equals_button);
}

CalculatorWidget::~CalculatorWidget()
{
}

void CalculatorWidget::add_button(GButton& button, Calculator::Operation operation)
{
    add_button(button);
    button.on_click = [this, operation](GButton&) {
        double argument = m_keypad.value();
        double res = m_calculator.begin_operation(operation, argument);
        m_keypad.set_value(res);
        update_display();
    };
}

void CalculatorWidget::add_button(GButton& button, int digit)
{
    add_button(button);
    button.set_text(String::number(digit));
    button.on_click = [this, digit](GButton&) {
        m_keypad.type_digit(digit);
        update_display();
    };
}

void CalculatorWidget::add_button(GButton& button)
{
    button.resize(35, 28);
}

void CalculatorWidget::update_display()
{
    m_entry->set_text(m_keypad.to_string());
    if (m_calculator.has_error())
        m_label->set_text("E");
    else
        m_label->set_text("");
}
