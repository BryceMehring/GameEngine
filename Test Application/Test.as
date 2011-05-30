
// Response
void CheckResponse(bool checked)
{
	if(checked)
	{
		print("Checked\n");
	}
	else
	{
		print("Not Checked\n");
	}
}
void CheckBoxResponce()
{

}

// todo: I should make scriptId a global property
void main(uint scriptId)
{
	CheckBoxData@ myCheckData = CheckBoxData();
	myCheckData.checked = false;
	myCheckData.name = "Console \n";
	
	myCheckData.pos.x = 0;
	myCheckData.pos.y = 50;

	myCheckData.func = GetId(@CheckResponse);
	myCheckData.scriptIndex = scriptId;

	uint id = ui.AddElement("CheckBox",@myCheckData);

	CheckBoxData@ myCheckData2 = CheckBoxData();
	myCheckData2.checked = false;
	myCheckData2.name = "Start Game \n";
	
	myCheckData2.pos.x = 200;
	myCheckData2.pos.y = 30;

	myCheckData2.func = GetId(@CheckResponse);
	myCheckData2.scriptIndex = scriptId;

	id = ui.AddElement("CheckBox",@myCheckData2);
}

  