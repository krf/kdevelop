{% load kdev_filters %}
/*

 {{ license|lines_prepend:" * " }}
 */


#include "{{ output_file_header }}"


#include <qtest_kde.h>


QTEST_KDEMAIN({{ name }}, NoGUI);


void {{ name }}::initTestCase()
{
    // Called before the first testfunction is executed
}


void {{ name }}::cleanupTestCase()
{
    // Called after the last testfunction was executed
}


void {{ name }}::init()
{
    // Called before each testfunction is executed
}


void {{ name }}::cleanup()
{
    // Called after every testfunction
}


{% for case in testCases %}

void {{ name }}::{{ case }}()
{


}

{% endfor %}

#include "{{ output_file_header|cut:".h"|cut:".hpp" }}.moc"
