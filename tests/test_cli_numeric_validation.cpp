#include <cstdlib>
#include <iostream>
#include <string>

static int run(const std::string &cmd)
{
    const int rc = std::system(cmd.c_str());
    if (rc == -1)
        return 127;
    return rc;
}

static void require(bool ok, const char *msg)
{
    if (!ok)
    {
        std::cerr << "FAIL: " << msg << "\n";
        std::exit(1);
    }
}

int main()
{
    // Build system places binaries under build/bin in this repo.
    // Use bash -lc so redirection/quoting works consistently on macOS CI runners.
    const std::string bin = "./build/bin/replay";

    // Non-finite should fail closed (exit 1) with invalid argument.
    int rc_nan = run("bash -lc '" + bin + " --input data/golden/itch_1m.bin --gap-ppm nan >/dev/null 2>&1'");
    require(rc_nan != 0, "--gap-ppm nan must fail");

    int rc_inf = run("bash -lc '" + bin + " --input data/golden/itch_1m.bin --corrupt-ppm inf >/dev/null 2>&1'");
    require(rc_inf != 0, "--corrupt-ppm inf must fail");

    // Negative should fail closed.
    int rc_neg = run("bash -lc '" + bin + " --input data/golden/itch_1m.bin --burst-ms -1 >/dev/null 2>&1'");
    require(rc_neg != 0, "--burst-ms -1 must fail");

    return 0;
}
