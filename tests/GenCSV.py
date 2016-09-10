import csv
import os

for exp in range(3, 4): #Experimento P, M ou G
	
	td = open('td' + str(exp) + '.csv', 'w');
	tc = open('tc' + str(exp) + '.csv', 'w');
	datad = open('datad' + str(exp), 'w');
	datac = open('datac' + str(exp), 'w');
	n = 30
	
	for esc in range(1, 4): #Escalonador 1, 2 ou 3

		sumd = sumc = 0;
		print("Iniciando testes no escalonador " + str(esc))
		datad.write('Escalonador '+ str(esc) +'---------------------------------' + "\n")
		datac.write('Escalonador '+ str(esc) +'---------------------------------' + "\n")	
		if (esc != 3):
			continue;
		
		for i in range(0, n):
			print("Teste " + str(i) + " com size " + str(exp * 10))
			os.system("java Generator " + str(exp * 10) + " 5 10 5 > in")
			os.system(".././ep1 " + str(esc) + " in out")
			out = open('out', 'r')
			for row in out: 
				continue
			row = row.split(' ')
			sumc += int(row[0])
			sumd += int(row[1])
			datad.write(str(row[1]) + "\n")
			datac.write(str(row[0]) + "\n")

		aved = float(sumd) / n
		avec = float(sumc) / n

		escal = "FCFS,"
		if (esc == 2): 
			escal = "SRTN,"
		if (esc == 3):
			escal = "M.FILAS,"
		td.write(escal + str(aved) + "\n")
		tc.write(escal + str(avec) + "\n")
	
	print("Terminei experimento " + str(exp));
	td.close()
	tc.close()
	datad.close()
	datac.close()

