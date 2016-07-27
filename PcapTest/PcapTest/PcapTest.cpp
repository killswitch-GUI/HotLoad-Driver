#define HAVE_REMOTE
#include <pcap.h>
#include <tchar.h>

int _tmain()
{
	pcap_if_t * allAdapters;
	pcap_if_t * adapter;
	char errorBuffer[PCAP_ERRBUF_SIZE];

	// retrieve the adapters from the computer
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL,
		&allAdapters, errorBuffer) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs_ex function: %s\n",
			errorBuffer);
		return -1;
	}

	// if there are no adapters, print an error
	if (allAdapters == NULL)
	{
		printf("\nNo adapters found! Make sure WinPcap is installed.\n");
		return 0;
	}

	// print the list of adapters along with basic information about an adapter
	int crtAdapter = 0;
	for (adapter = allAdapters; adapter != NULL; adapter = adapter->next)
	{
		printf("\n%d.%s ", ++crtAdapter, adapter->name);
		printf("-- %s\n", adapter->description);
	}

	printf("\n");

	// free the adapter list
	pcap_freealldevs(allAdapters);

	system("PAUSE");
	return 0;
}