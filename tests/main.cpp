#include <moka/moka.h>
#include <webframe/webframe.hpp>

#include <stdio.h>

#include <UT/main.hpp>
#include <IT/main.hpp>

int main ()
{
	static_assert(webframe::webframe::init(), "constexpr initiation failed");
	
	std::cout << "===============================  Testing  ===============================" << "\n";

	Moka::Report report;
	testCase_UnitTests(report);
	testCase_IntegrationTests(report);
	return report.print();
}