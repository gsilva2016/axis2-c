#include <CuTest.h>
#include "../../util/test/util_test.h"
#include "../../common/test/common_test.h"

void RunAllTests(void) {
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, axis2_utilGetSuite());
    CuSuiteAddSuite(suite, axis2_commonGetSuite());
    CuSuiteAddSuite(suite, axis2_omGetSuite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(void) {
    RunAllTests();
    return 0;
}
