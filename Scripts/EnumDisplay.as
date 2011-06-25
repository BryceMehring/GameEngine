/*void SetDisplayMode(uint n)
{
	renderer.SetDisplayMode(n);
}*/

void main()
{
	//ListData@ data = ListData();

	uint n = renderer.EnumerateDisplayAdaptors();

	string modes;
	for(uint i = 0; i < n; ++i)
	{
		modes += renderer.GetDisplayModeStr(i);
		modes += "\n";
		//print("\n");
		//print(renderer.GetDisplayModeStr(i));
		//print("\n");
	}
	print(modes);
}