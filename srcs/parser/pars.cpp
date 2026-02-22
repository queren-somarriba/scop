bool	parsFile()
{
	FILE	*file = fopen("model.obj", "r");
	char	line[128];

	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == 'v' && line[1] == ' ')
			parsVertex();// with sscanf
		else if (line[0] == 'f' && line[1] == ' ')
			ParsFace()// pars indexes
	}
	fclose(file);
}

bool parsVertex(std::string &str)
{
		if (sscanf(str.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3)
		return (std::cerr << "Format invalide ! Attendu : x y z" << std::endl, 1);
}