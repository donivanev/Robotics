class APIClient {
    async getLatestData() {
        const res = await fetch('http://192.168.1.101:8080/api/events/latest')
        const data = await res.json()
        return data
    }
};

const apiClient = new APIClient()

export default apiClient