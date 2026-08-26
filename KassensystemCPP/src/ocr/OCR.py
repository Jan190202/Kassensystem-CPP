import consumptionModule

def readDocument(imagePath):
	results = [];

	result1 = consumptionModule.ConsumptionInputs() 
	result1.personName = "Dieter Schwarz"
	result1.nBeer05 = 3
	result1.nBeer04 = 1
	result1.nSoftdrinks = 0   
	result1.nWater = 0
	result1.otherExpense = 5.2 

	result2= consumptionModule.ConsumptionInputs() 
	result2.personName = "Albrecht Adams"
	result2.nBeer05 = 4
	result2.nBeer04 = 0
	result2.nSoftdrinks = 0   
	result2.nWater = 0
	result2.otherExpense = 3

	result3= consumptionModule.ConsumptionInputs() 
	result3.personName = "Sophia Ströbel"
	result3.nBeer05 = 0
	result3.nBeer04 = 0
	result3.nSoftdrinks = 1   
	result3.nWater = 0
	result3.otherExpense = 0

	results.append(result1)
	results.append(result2)
	results.append(result3)

	return results