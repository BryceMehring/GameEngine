
array<string> ratioName = {"16:10","16:9"};
array<float>  ratios = {16/10.0f,16/9.0f};
/*void SetDisplayMode(uint n)
{
	renderer.SetDisplayMode(n);
}*/

void main()
{
	

	//CheckBoxData@ myCheckData = CheckBoxData();
	//myCheckData.func = as.GetFunc(@SetDisplayMode);
	renderer.EnumerateDisplayAdaptors();
	uint ratioSize = renderer.GetRatioSize();

	for(uint i = 0; i < ratioSize; ++i)
	{
		float ratio = renderer.GetRatio(i);
		uint modeSize = renderer.GetModeSize(ratio);

		for(int j = 0; j < modeSize; ++j)
		{
			modes += ratio;
			modes += renderer.GetDisplayModeStr(ratio,i);
			modes += "\n";
		}
	}



	/*
	uint n = renderer.GetModeSize(ratios[1]);

	string modes = ratioName[1] + "\n";
	
	for(uint i = 0; i < n; ++i)
	{
		modes += renderer.GetDisplayModeStr(ratios[1],i);
		modes += "\n";
		//print("\n");
		//print(renderer.GetDisplayModeStr(i));
		//print("\n");
	}
	console.print(modes,0xffffffff);*/
}