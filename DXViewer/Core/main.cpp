
void printHelpMessage() 
{
	std::cerr << "Usage: " << "DXViewer" << " <option(s)>\n"
		<< "Options:\n"
		<< "\t--help\t\tShow this help message\n"
		<< "\t--benchmark\tPrint benchmark messages\n"
		<< std::endl;
}

int main(int argc, char** argv)
{
	try {
		// Parse command line arguments and configure application settings.
		// Note: command line arguments shall always have higher priority than configuration files.
		for (General::Type::Int i = 1; i < argc; ++i) {
			std::string arg = argv[i];
			if ((arg == "-h") || (arg == "--help")) {
				printHelpMessage();
				return EXIT_SUCCESS;
			}
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception& exception) 
	{
		std::cerr << "FATAL: " << exception.what() << std::endl;
	}
	catch (...) 
	{
		std::cerr << "FATAL: Caught unhandled exception!" << std::endl;
		throw;
	}

	return EXIT_FAILURE;
}