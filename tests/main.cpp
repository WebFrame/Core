#include <moka/moka.h>
#include <core/core.hpp>

#include <stdio.h>

#include <UT/main.hpp>
#include <IT/main.hpp>

int main ()
{
	static_assert(webframe::core::application::init(), "constexpr initiation failed");
	
	std::cout << "===============================  Testing  ===============================" << "\n";

	Moka::Report report;
	testCase_UnitTests(report);
	testCase_IntegrationTests(report);
	return report.print();
}